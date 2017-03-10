static const uint compute_kernel_width = 1;
static const uint compute_kernel_height = 1;
static const uint compute_kernel_depth = 1;

[numthreads(compute_kernel_width, compute_kernel_height, compute_kernel_depth)]
void cs_main()
{
}
