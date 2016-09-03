#include "cg/math/math.h"


namespace cg {

const float_3 float_3::zero {};
const float_3 float_3::unit_x(1, 0, 0);
const float_3 float_3::unit_y(0, 1, 0);
const float_3 float_3::unit_z(0, 0, 1);
const float_3 float_3::unit_xyz(1);

const float_4 float_4::zero {};
const float_4 float_4::unit_x(1, 0, 0, 0);
const float_4 float_4::unit_y(0, 1, 0, 0);
const float_4 float_4::unit_z(0, 0, 1, 0);
const float_4 float_4::unit_w(0, 0, 0, 1);
const float_4 float_4::unit_xyzw(1);

} // namespace cg