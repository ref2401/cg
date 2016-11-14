#ifndef CG_FILE_FILE_IMAGE_H_
#define CG_FILE_FILE_IMAGE_H_

#include "cg/data/image.h"
#include "cg/file/file_base.h"


namespace cg {
namespace file {

cg::data::Image_2d load_image_tga(const std::string& filename);

cg::data::Image_2d load_image_tga(const char* filename);

} // namespace file
} // namespace cg

#endif // CG_FILE_FILE_IMAGE_H_
