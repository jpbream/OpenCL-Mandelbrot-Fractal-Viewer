
__kernel void func(write_only image2d_t image, double ox, double oy, double s, int threshold, int numIterations, __global float* colors)
{
	int width = get_global_size(0);
	int height = get_global_size(1);

	int x = get_global_id(0);
	int y = get_global_id(1);

	double left = -1.75 / s + ox;
	double right = 1.75 / s + ox;
	double bottom = -1 / s + oy;
	double top = 1 / s + oy;

	double ax = (double)x / width;
	double px = left * (1 - ax) + right * ax;

	double ay = (double)y / height;
	double py = bottom * (1 - ay) + top * ay;

	double zr = 0;
	double zi = 0;

	double cr = px;
	double ci = py;

	int i;
	for (i = 0; i < numIterations - 1 && zr * zr + zi * zi < threshold * threshold; ++i)
	{
		double zrTemp = zr * zr - zi * zi + cr;
		zi = 2 * zr * zi + ci;

		zr = zrTemp;
	}

	write_imagef(image, (int2)(x, y), (float4)(colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2], 1));
}