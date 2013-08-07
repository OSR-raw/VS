#include "KinectTool.h"
#include "TriangleMesh.h"
#include "KinectReader.h"
#include "GridModel.h"


KinectTool::KinectTool()
{
}

KinectTool::KinectTool( float half_x, float half_y, float start_z, float end_z )
{
	_msh = new TriangleMesh(640, 480, -half_x, -half_y, half_x, half_y, start_z);
	_start_z = start_z;
	_end_z = end_z;
	int start_d = 800;
	_reader = new KinectReader(start_d, start_d+int(_start_z - _end_z), _start_z - _end_z );
	_tmp_blured_image = new float[640*480];
	memset( _tmp_blured_image, 0, 640*480*sizeof(float) );
}

inline void Blur( float* in_out, float* tmp )
{
	//static double coeffs[] = {0.0545, 0.224, 0.4026, 0.224, 0.0545};
	static double coeffs[] = {1.0/5.0, 1.0/5.0, 1.0/5.0, 1.0/5.0, 1.0/5.0};
	double summ = 0;
	//X

	for ( int j = 2; j < 478; j++ )	
	{
		for ( int i = 0; i < 640; i++ )
		{
			summ = 0.0f;
			for ( int dx = -2; dx < 3; dx++ )
			{
				summ += coeffs[dx+2]*in_out[ (j+dx)*640 + i ];
				//summ += coeffs[dx+2]*( fabsf(in_out[ (j+dx)*640 + i ] - in_out[ j*640 + i ] ) > 10.0f  ? in_out[ j*640 + i ] : in_out[ (j+dx)*640 + i ] );
			}
			//tmp[ j*640 + i ] = (summ + tmp[ j*640 + i ])/2.0f;
			tmp[ j*640 + i ]  = summ;
		}
	}	
	//Y
	for ( int j = 2; j < 478; j++ )
	{
		for ( int i = 2; i < 638; i++ )
		{
			summ = 0.0f;
			for ( int dx = -2; dx < 3; dx++ )
			{
				summ += (coeffs[dx+2]*tmp[ j*640 + i +dx]);
				//summ += coeffs[dx+2]*( fabsf(tmp[ j*640 + i +dx] - tmp[ j*640 + i ] ) > 10.0f  ? tmp[ j*640 + i ] : tmp[ j*640 + i +dx]);
			}
			in_out[ j*640 + i ] = summ;
		}
	}	
}

void KinectTool::DoToolUpdate()
{
	_reader->ProcessDepth();//both can be paralelized, but
	//sync point here for process depth.
	//memfence, in a way
	float* depth_ptr = _reader->GetDepth();//Flikers
	//Blur.	
	Blur( depth_ptr, _tmp_blured_image );//Makes it emen more flikering, but it should be done.
	//memfence again
	_msh->UpdateDepth( depth_ptr );
	//memfence again
	//or you can combine both calls into one ( basically same happens there ), BUT with blur I see no way to do it.
	//So the best way to parralelize it - SIMD-like paralelization inside of each function.
}

inline Point Rotate( const Point& v, const glm::quat& q )//looks to be fast.
{
	//quat - point cross
	Point res;
	res.coord[0] = q.y * v.coord[2] - v.coord[1] * q.z;
	res.coord[1] = q.z * v.coord[0] - v.coord[2] * q.x;
	res.coord[2] = q.x * v.coord[1] - v.coord[0] * q.y;
	//again cross
	static Point res1;
	res1.coord[0] = q.y * res.coord[2] - res.coord[1] * q.z;
	res1.coord[1] = q.z * res.coord[0] - res.coord[2] * q.x;
	res1.coord[2] = q.x * res.coord[1] - res.coord[0] * q.y;
	//
	res.coord[0] *= 2.0f*q.w;
	res.coord[1] *= 2.0f*q.w;
	res.coord[2] *= 2.0f*q.w;
	//
	res1.coord[0] *= 2.0f;
	res1.coord[1] *= 2.0f;
	res1.coord[2] *= 2.0f;
	
	res.coord[0] += res1.coord[0] + v.coord[0];
	res.coord[1] += res1.coord[1] + v.coord[1];
	res.coord[2] += res1.coord[2] + v.coord[2];
	return res;
}


int KinectTool::InteractModel( GridModel* model, glm::quat quat )
{
	//for loop for each point, rotated by inverse of quat
	Point* points = _msh->GetPoints();
	glm::quat inverse = glm::conjugate(quat);

	//glm::vec3 rot(0, -90.0f*glm::pi<float>()/180.0, 0);
	//glm::quat inverse =glm::quat(rot);


	Point action_point;
	unsigned int index = 0;
	unsigned int tmp1, tmp2, tmp3;
	unsigned int grid_size = model->GetSize();
	unsigned int grid_dimm = model->GetDimm() - 1;

	unsigned int pad_depth = grid_dimm;

	Color clr;
	clr.comp[0] = 0;
	clr.comp[1] = 0;
	clr.comp[2] = 0;
	clr.comp[3] = 0;
	int accum = 0;
	Point tmp;
	Point dir_vector;
	const float dir_z_step = 1.0f;
	dir_vector.coord[0] = 0.0f;
	dir_vector.coord[1] = 0.0f;
	dir_vector.coord[2] = dir_z_step;
	dir_vector = Rotate(dir_vector, inverse );
	unsigned int tmp_pad = 0;
	for ( unsigned int i = 0; i < 640; i++ )
	{
		for ( unsigned int j = 0; j < 480; j++ )
		{
			//tmp = points[ i*480 + j ];
			//tmp.coord[2] -= dir_z_step*pad_depth;
			action_point = Rotate( points[ i*480 + j ], inverse);
			for ( unsigned int delta = 0; delta < pad_depth; delta++ )
			{
				tmp.coord[0] = action_point.coord[0] + dir_vector.coord[0]*delta;
				tmp.coord[1] = action_point.coord[1] + dir_vector.coord[1]*delta;
				tmp.coord[2] = action_point.coord[2] + dir_vector.coord[2]*delta;
				index = model->GetCellIndex(tmp, tmp1, tmp2, tmp3);

				if ( index < grid_size )
				{				
					if (!( ( tmp1 > grid_dimm ) || ( tmp2 > grid_dimm ) || ( tmp3 > grid_dimm )))
						accum += model->UpdateCell(tmp1, tmp2, tmp3, &clr);
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
	}
	return accum;
}


TriangleMesh* KinectTool::GetToolMesh()
{
	return _msh;
}


KinectTool::~KinectTool()
{
	delete _msh;
	delete _reader;
	delete _tmp_blured_image;
}
