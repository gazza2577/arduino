/*! \file */

#ifndef __NEONEXTION_NEXTIONPICTURE
#define __NEONEXTION_NEXTIONPICTURE

#include "NeoNextion.h"
#include "INextionTouchable.h"

/*!
 * \class NextionPicture
 * \brief Represents a full size picture widget.
 */
class NextionPicture : public INextionTouchable
{
public:
  NextionPicture(Nextion &nex, uint8_t page, uint8_t component,
                 const std::string &name);

  uint16_t getPictureID();
  bool setPictureID(uint16_t id);
};

#endif
