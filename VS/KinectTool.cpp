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
	_reader = new KinectReader(start_d, start_d+int(_start_z - _end_z)*3, _start_z - _end_z );
}

void KinectTool::DoToolUpdate()
{
	_reader->ProcessDepth();//both can be paralelized, but
	//sync point here for process depth.
	//memfence, in a way
	_msh->UpdateDepth(_reader->GetDepth());
	//memfence again
	//or you can combine both calls into one ( basically same happens there )
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

inline float interpolate( Point* points, int x, int y )
{
	
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

	Color clr;
	clr.comp[0] = 0;
	clr.comp[1] = 0;
	clr.comp[2] = 0;
	clr.comp[3] = 0;
	int accum = 0;
	for ( unsigned int i = 0; i < 640; i++ )
	{
		for ( unsigned int j = 0; j < 480; j++ )
		{
			action_point = Rotate( points[ i*480 + j ], inverse);
			index = model->GetCellIndex(&action_point, tmp1, tmp2, tmp3);

			//if ( index < grid_size )
			{				
				if (!( ( tmp1 > grid_dimm ) || ( tmp2 > grid_dimm ) || ( tmp3 > grid_dimm )))
					accum += model->UpdateCell(tmp1, tmp2, tmp3, &clr);
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
}
