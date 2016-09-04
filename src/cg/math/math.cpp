#include "cg/math/math.h"


namespace cg {

const float3 float3::unit_x(1, 0, 0);
const float3 float3::unit_y(0, 1, 0);
const float3 float3::unit_z(0, 0, 1);
const float3 float3::unit_xyz(1);
const float3 float3::zero(0);

const float4 float4::unit_x(1, 0, 0, 0);
const float4 float4::unit_y(0, 1, 0, 0);
const float4 float4::unit_z(0, 0, 1, 0);
const float4 float4::unit_w(0, 0, 0, 1);
const float4 float4::unit_xyzw(1);
const float4 float4::zero(0);

const quat quat::i(1, 0, 0, 0);
const quat quat::j(0, 1, 0, 0);
const quat quat::k(0, 0, 1, 0);
const quat quat::identity(0, 0, 0, 1);
const quat quat::zero(0, 0, 0, 0);

} // namespace cg