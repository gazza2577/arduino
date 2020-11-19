/**********************************************************************
ESP32 COMMAND STATION

COPYRIGHT (c) 2017-2020 Mike Dunston

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses
**********************************************************************/

#include "FileSystemManager.h"
#include "LCCStackManager.h"

#include <algorithm>
#include <dirent.h>
#include <driver/sdmmc_defs.h>
#include <driver/sdmmc_host.h>
#include <driver/sdmmc_types.h>
#include <driver/sdspi_host.h>
#include <esp_spiffs.h>
#include <esp_vfs_fat.h>
#include <freertos_drivers/esp32/Esp32Gpio.hxx>
#include <sdmmc_cmd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utils/FileUtils.hxx>

static constexpr const char FACTORY_RESET_MARKER_FILE[] = "resetcfg.txt";

#if defined(CONFIG_ESP32CS_FORCE_FACTORY_RESET_PIN) && CONFIG_ESP32CS_FORCE_FACTORY_RESET_PIN >= 0
#include <freertos_drivers/esp32/Esp32Gpio.hxx>
GPIO_PIN(FACTORY_RESET, GpioInputPU, CONFIG_ESP32CS_FORCE_FACTORY_RESET_PIN);
#else
#include <freertos_drivers/arduino/DummyGPIO.hxx>
typedef DummyPinWithReadHigh FACTORY_RESET_Pin;
#endif // CONFIG_ESP32CS_FORCE_FACTORY_RESET_PIN

// Helper which converts a string to a uint64 value.
uint64_t string_to_uint64(std::string value)
{
  // remove period characters if present
  value.erase(std::remove(value.begin(), value.end(), '.'), value.end());
  // convert the string to a uint64_t value
  return std::stoull(value, nullptr, 16);
}

void recursiveWalkTree(const string &path, bool remove = false, bool first = true)
{
  if (first && !remove)
  {
    LOG(INFO, "[FS] Dumping content of filesystem: %s", path.c_str());
  }
  DIR *dir = opendir(path.c_str());
  if (dir)
  {
    dirent *ent = NULL;
    while ((ent = readdir(dir)) != NULL)
    {
      string fullPath = path + "/" + ent->d_name;
      if (ent->d_type == DT_REG)
      {
        struct stat statbuf;
        stat(fullPath.c_str(), &statbuf);
        if (remove)
        {
          LOG(VERBOSE, "[FS] Deleting %s (%lu bytes)", fullPath.c_str()
            , statbuf.st_size);
          ERRNOCHECK(fullPath.c_str(), unlink(fullPath.c_str()));
        }
        else
        {
          // NOTE: not using LOG(INFO, ...) here due to ctime injecting a
          // newline at the end of the string.
          printf("[FS] %s (%lu bytes) mtime: %s", fullPath.c_str()
               , statbuf.st_size, ctime(&statbuf.st_mtime));
        }
      }
      else if (ent->d_type == DT_DIR)
      {
        recursiveWalkTree(fullPath, remove, false);
      }
    }
    closedir(dir);
    if (remove)
    {
      rmdir(path.c_str());
    }
  }
  else
  {
    LOG_ERROR("[FS] Failed to open directory: %s", path.c_str());
  }
}

static constexpr uint64_t ONE_MB = 1048576ULL;
FileSystemManager::FileSystemManager()
{
#if CONFIG_ESP32CS_FORCE_FACTORY_RESET
  bool factory_reset_config = true;
#else
  bool factory_reset_config = false;
#endif

  FACTORY_RESET_Pin::hw_init();

  if (FACTORY_RESET_Pin::instance()->is_clr())
  {
    factory_reset_config = true;
  }

  // Use SPI mode instead of SDMMC due to TTGO-T1 failing to mount the SD card.
  esp_err_t err = ESP_FAIL;
  sdmmc_host_t sd_host = SDSPI_HOST_DEFAULT();
  esp_vfs_fat_mount_config_t sd_cfg =
  {
    .format_if_mount_failed = true,
    .max_files = 10,
    .allocation_unit_size = 0
  };
  gpio_set_pull_mode(GPIO_NUM_2, GPIO_PULLUP_ONLY);
  gpio_set_pull_mode(GPIO_NUM_13, GPIO_PULLUP_ONLY);
  gpio_set_pull_mode(GPIO_NUM_14, GPIO_PULLUP_ONLY);
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4,2,0)
  sdspi_slot_config_t sd_slot = SDSPI_SLOT_CONFIG_DEFAULT();
  err = ESP_ERROR_CHECK_WITHOUT_ABORT(esp_vfs_fat_sdmmc_mount(VFS_MOUNT, &sd_host, &sd_slot, &sd_cfg, &sd_));
#else
  spi_bus_config_t bus_cfg =
  {
    .mosi_io_num = GPIO_NUM_15,
    .miso_io_num = GPIO_NUM_2,
    .sclk_io_num = GPIO_NUM_14,
    .quadwp_io_num = GPIO_NUM_NC,
    .quadhd_io_num = GPIO_NUM_NC,
    .max_transfer_sz = 4000,
    .flags = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_MOSI,
    .intr_flags = 0
  };
  sdspi_device_config_t device_cfg = SDSPI_DEVICE_CONFIG_DEFAULT();
  err = ESP_ERROR_CHECK_WITHOUT_ABORT(spi_bus_initialize(HSPI_HOST, &bus_cfg, 1));
  if (err == ESP_OK)
  {
    err = ESP_ERROR_CHECK_WITHOUT_ABORT(esp_vfs_fat_sdspi_mount(VFS_MOUNT, &sd_host, &device_cfg, &sd_cfg, &sd_));
  }
#endif
  if (err == ESP_OK)
  {
    float capacity = ((uint64_t)sd_->csd.capacity) * sd_->csd.sector_size;
    LOG(INFO, "[FS] SD card '%s' mounted, max capacity %.2f MB"
      , sd_->cid.name, (float)(capacity / ONE_MB));

    FATFS *fs;
    DWORD c;
    if (f_getfree("0:", &c, &fs) == FR_OK)
    {
      float used_space =
        ((uint64_t)fs->csize * (fs->n_fatent - 2 - fs->free_clst)) * fs->ssize;
      float max_space = ((uint64_t)fs->csize * (fs->n_fatent - 2)) * fs->ssize;
      LOG(INFO, "[FS] SD FAT usage: %.2f/%.2f MB",
          (float)(used_space / ONE_MB), (float)(max_space / ONE_MB));
    }
    LOG(INFO, "[FS] SD will be used for persistent storage.");
  }
  else
  {
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4,2,0)
    // unmount the SD VFS since it failed to successfully mount. We will
    // remount SPIFFS in it's place instead.
    esp_vfs_fat_sdmmc_unmount();
#endif
    LOG(INFO, "[FS] SD Card not present or mounting failed, using SPIFFS");
    esp_vfs_spiffs_conf_t conf =
    {
      .base_path = VFS_MOUNT,
      .partition_label = NULL,
      .max_files = 10,
      .format_if_mount_failed = true
    };
    // Attempt to mount the partition
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));
    // check that the partition mounted
    size_t total = 0, used = 0;
    if (esp_spiffs_info(NULL, &total, &used) == ESP_OK)
    {
      LOG(INFO, "[FS] SPIFFS usage: %.2f/%.2f KiB", (float)(used / 1024.0f)
        , (float)(total / 1024.0f));
    }
    else
    {
      LOG_ERROR("[FS] Unable to retrieve SPIFFS utilization statistics.");
    }
    LOG(INFO, "[FS] SPIFFS will be used for persistent storage.");
  }

  if (exists(FACTORY_RESET_MARKER_FILE))
  {
    factory_reset_config = true;
  }

  if (factory_reset_config)
  {
    LOG(WARNING, "!!!! WARNING WARNING WARNING WARNING WARNING !!!!");
    LOG(WARNING, "!!!! WARNING WARNING WARNING WARNING WARNING !!!!");
    LOG(WARNING, "!!!! WARNING WARNING WARNING WARNING WARNING !!!!");
    LOG(WARNING,
        "[FS] The factory reset flag has been set to true, all persistent "
        "data will be cleared.");
    uint8_t countdown = 10;
    while (--countdown)
    {
      LOG(WARNING, "[FS] Factory reset will be initiated in %d seconds..."
        , countdown);
      usleep(SEC_TO_USEC(1));
    }
    LOG(WARNING, "[FS] Factory reset starting!");
  }

  recursiveWalkTree(VFS_MOUNT, factory_reset_config);
  // Pre-create ESP32 CS configuration directory.
  mkdir(CS_CONFIG_DIR, ACCESSPERMS);
  // Pre-create LCC configuration directory.
  mkdir(LCC_CFG_DIR, ACCESSPERMS);
}

void FileSystemManager::shutdown()
{
  // Unmount the SPIFFS partition
  if (esp_spiffs_mounted(NULL))
  {
    LOG(INFO, "[FS] Unmounting SPIFFS...");
    ESP_ERROR_CHECK(esp_vfs_spiffs_unregister(NULL));
  }

  // Unmount the SD card if it was mounted
  if (sd_)
  {
    LOG(INFO, "[FS] Unmounting SD...");
    ESP_ERROR_CHECK(esp_vfs_fat_sdmmc_unmount());
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4,2,0)
    ESP_ERROR_CHECK(spi_bus_free(HSPI_HOST));
#endif
  }
}

bool FileSystemManager::exists(const string &name)
{
  struct stat statbuf;
  string configFilePath = getFilePath(name);
  LOG(VERBOSE, "[FS] Checking for %s", configFilePath.c_str());
  // this code is not using access(path, F_OK) as that is not available for
  // SPIFFS VFS. stat(path, buf) does work though.
  return !stat(configFilePath.c_str(), &statbuf);
}

void FileSystemManager::remove(const string &name)
{
  string configFilePath = getFilePath(name);
  LOG(VERBOSE, "[FS] Removing %s", configFilePath.c_str());
  unlink(configFilePath.c_str());
}

string FileSystemManager::load(const string &name)
{
  string configFilePath = getFilePath(name);
  if (!exists(name))
  {
    LOG_ERROR("[FS] %s does not exist, returning blank json object"
            , configFilePath.c_str());
    return "{}";
  }
  LOG(VERBOSE, "[FS] Loading %s", configFilePath.c_str());
  return read_file_to_string(configFilePath);
}

void FileSystemManager::store(const char *name, const string &content)
{
  string configFilePath = getFilePath(name);
  LOG(VERBOSE, "[FS] Storing %s, %d bytes", configFilePath.c_str()
    , content.length());
  write_string_to_file(configFilePath, content);
}

string FileSystemManager::getFilePath(const string &name)
{
  return StringPrintf("%s/%s", CS_CONFIG_DIR, name.c_str());
}

void FileSystemManager::force_factory_reset()
{
  LOG(INFO, "[FS] Enabling forced factory_reset.");
  string marker = "force factory reset";
  store(FACTORY_RESET_MARKER_FILE, marker);
  Singleton<esp32cs::LCCStackManager>::instance()->reboot_node();
}
