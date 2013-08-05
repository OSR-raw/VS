#include "main.h"

class Input
{
public:
	Input();
	~Input();
	void OnKeyPressed( char c );
	void OnMouseLBDown( int x, int y );
	void OnMouseMove( int dx, int dy );
	void OnMouseLBUp( int x, int y );
	void OnSroll( int dx );
	glm::mat4 GetObjectM();
	glm::quat GetObjectQ();
	glm::mat4 GetViewM();
	void SetViewM(const glm::mat4& mat );
	void SetZoom( float val );
private:
	int _mouseStartX;
	int _mouseStartY;
	int _prevX;
	int _prevY;
	bool _lbtn_pressed;
	float _angleXS;
	float _angleX;
    float _angleYS;
	float _angleY;

	//glm::mat4 _obj_mat;
	glm::quat _obj_quat;
	glm::mat4 _view_mat;
	float zoom_val;
};

