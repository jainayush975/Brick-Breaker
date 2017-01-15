#include<bits/stdc++.h>

using namespace std;

bool Collide(double lx , double ly , int angle , double bx , double by) {
  double lx1 = lx + BeamLength * cos(angle * M_PI / 180.0f), m = tan(angle * M_PI /108.0f);
  double c = ly - (m * lx);
  if( (((lx-bx+2)*(lx1-bx+2))>0) && (((lx-bx-2)*(lx1-bx-2))>0))
    return false;
  else if ( (((by-(m*bx)-c)*(by-(m*bx)-c+BrickLength))>0) )
    return false;
  else
    return true;
}



int main()
{
	cin << x << y << 
	cout << arr[2] << " " << mp[1][2] << endl;
	return 0;
}
