 #include <windows.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "myHeader.h"
#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <map>
#include <windows.h>
#include <time.h>
#include <math.h>
#include "SOIL.h"
//////////////////////////////////////
// identificatori 
GLuint
  VaoId, 
  VboId, 
  EboId, 
  ColorBufferId,
  ProgramId,
  myMatrixLocation,
  viewLocation,
  projLocation,
  matrRotlLocation,
  codColLocation,
  depthLocation;

 GLuint texture;


#define tex_sz 32 
using namespace std;
double ofi,saunuofi;
vector<string> toggledSlots;
class gameObject;
 class rect3
 {
 public:
	 float mx,MX,my,MY,mz,MZ;
	 float omx,OMX,omy,OMY,omz,OMZ;
	 gameObject* target;
	 void (*calBack)(gameObject*, rect3);
	 rect3(float _mx,float _MX,float _my,float _MY,float _mz,float _MZ,void (*f)(gameObject*,rect3),gameObject* care);

 };
  class baundingBox
 {
 public:
	 vector<rect3> C;
	 baundingBox();
	 int Add(rect3 x);
	 bool insideCollisionZone(int a);
	 vector<int> getLocation();
	 bool closeToCollisionZone(int a,float thresh);
	 vector<int> undeSuntCloseEnough(float veigar);
	 void trigger(int x);
	 void triggerAllInternal(){
		 vector<int> care=getLocation();
		 for(int i=0;i<care.size();i++)trigger(care[i]);
	 }
	 void triggerAllClose(float veigar){
		 vector<int> care=undeSuntCloseEnough(veigar);
		 for(int i=0;i<care.size();i++)trigger(care[i]);
	 }
} permise;
 class gameObject{
 public:
	 baundingBox colliders;
	 string nume;
	 string real_name;
	 vector<GLfloat> orig_vf;
	 vector<GLfloat> data;
	 vector<GLuint> coord;
	 gameObject(GLfloat* vf, int nvf, GLuint* cd, int ncd, string eu);
	 gameObject();
	 GLfloat _sx;
	 GLfloat _sy;
	 GLfloat _sz;
	 GLfloat _tx;
	 GLfloat _ty;
	 GLfloat _tz;
	 GLfloat _fi;
	 GLfloat _theta;
	 void update_base();
	 virtual void update();
	 virtual gameObject* deepCopy(){
		 gameObject* trb=new gameObject();
		 trb->nume=nume;
		 trb->real_name=real_name;
		 trb->orig_vf=orig_vf;
		 trb->data=data;
		 trb->coord=coord;
		  trb->_tx=_tx;trb->_ty=_ty;trb->_tz=_tz;
		  trb->_sx=_sx;trb->_sy=_sy;trb->_sz=_sz;
		  trb->_fi=_fi;trb->_theta=_theta;
		  trb->colliders = baundingBox();
		  trb->colliders.C = vector<rect3>();
		  for(int i=0;i<colliders.C.size();i++){
			  trb->colliders.C.push_back(colliders.C[i]);
			  //if(colliders.C[colliders.C.size()-1].target==&rhs){
				  trb->colliders.C[trb->colliders.C.size()-1].target=trb;
			  //}
		  }
		  return trb;
	 }
	 gameObject(gameObject* rhs){
		 nume=rhs->nume;
		 real_name=rhs->real_name;
		  orig_vf=rhs->orig_vf;
		  data=rhs->data;
		  coord=rhs->coord;
		  _tx=rhs->_tx;_ty=rhs->_ty;_tz=rhs->_tz;
		  _sx=rhs->_sx;_sy=rhs->_sy;_sz=rhs->_sz;
		  _fi=rhs->_fi;_theta=rhs->_theta;
		  colliders = baundingBox();
		  colliders.C = vector<rect3>();
		  for(int i=0;i<rhs->colliders.C.size();i++){
			  colliders.C.push_back(rhs->colliders.C[i]);
			  //if(colliders.C[colliders.C.size()-1].target==&rhs){
				  colliders.C[colliders.C.size()-1].target=this;
			  //}
		  }
	 }
	 virtual void si_dupa(){
		 return;
		 _sx=_sy=_sz=1.0f;
		 _tx=_ty=_tz=0;
		 _fi=0;
	 }
 };
 class lista_trebi{
 public:
	 int img_height;
	 int img_width;
	 vector<GLfloat> multi;
	 vector<GLuint> multe;
	 vector<gameObject*> children;
	 lista_trebi(){};
	 ~lista_trebi(){}
	 lista_trebi(lista_trebi* rhs){
		 children.clear();
		 children=vector<gameObject*>();
		 img_height=rhs->img_height;
		 img_width=rhs->img_width;
		 for(int i=0;i<rhs->children.size();i++){
			 //printf("%x ",rhs->children[i]);
			 children.push_back(rhs->children[i]->deepCopy());
		 }
	 }
	 vector<gameObject*> findObject(string ce){
		 vector<gameObject*> res;
		 for(int i=0;i<children.size();i++){
			 if(strstr(children[i]->nume.c_str(),ce.c_str())!=NULL)
				 res.push_back(children[i]);
			 lista_trebi* el=dynamic_cast<lista_trebi*>(children[i]);
			 if(el){
				 vector<gameObject*> toadd=el->findObject(ce);
				 for(int ii=0;ii<toadd.size();ii++)
					 res.push_back(toadd[ii]);
				 toadd.clear();
			 }
		 }
		 return res;
	 }
	 int numVertices();
	 int numTriangles();
	 void CreateVBO();
	 void UpdateBuffer();
 } ;
 class compositeGameObject: public gameObject,public lista_trebi{
 public:
	 baundingBox proprii;
	 compositeGameObject();
	 compositeGameObject(compositeGameObject* rhs):
		 gameObject(rhs),
		 lista_trebi(rhs){
		for(int i=0;i<rhs->proprii.C.size();i++){
			proprii.C.push_back(rhs->proprii.C[i]);
			proprii.C[proprii.C.size()-1].target=this;
		}
	 
	 };
	 void propaga(string ce){
		 static char buff[999];
		 for(int i=0;i<children.size();i++){
			 sprintf(buff,"%s.%s",ce.c_str(),children[i]->nume.c_str());
			 children[i]->nume=string(buff);
			 compositeGameObject* ala=dynamic_cast<compositeGameObject*>(children[i]);
			 if(ala)
				 ala->propaga(real_name);
		 }
	 };
	 void update_lista();
	 gameObject* deepCopy(){
		 compositeGameObject* trb=new compositeGameObject();
		 trb->nume=nume;
		 trb->real_name=real_name;
		 trb->children.clear();
		 trb->children=vector<gameObject*>();
		 trb->img_height=img_height;
		 trb->img_width=img_width;
		 for(int i=0;i<children.size();i++)
			 trb->children.push_back(children[i]->deepCopy());
		 trb->orig_vf=orig_vf;
		 trb->data=data;
		 trb->coord=coord;
		  trb->_tx=_tx;trb->_ty=_ty;trb->_tz=_tz;
		  trb->_sx=_sx;trb->_sy=_sy;trb->_sz=_sz;
		  trb->_fi=_fi;trb->_theta=_theta;
		  trb->colliders = baundingBox();
		  trb->colliders.C = vector<rect3>();
		  for(int i=0;i<colliders.C.size();i++){
			  trb->colliders.C.push_back(colliders.C[i]);
			  //if(colliders.C[colliders.C.size()-1].target==&rhs){
				  trb->colliders.C[trb->colliders.C.size()-1].target=trb;
			  //}
		  }
		  for(int i=0;i<proprii.C.size();i++){
			trb->proprii.C.push_back(proprii.C[i]);
			trb->proprii.C[trb->proprii.C.size()-1].target=trb;///???
		  }
		  return trb;
	 }

	 void update();
	 void si_dupa(){

	 };
	 /*compositeGameObject& operator=(const compositeGameObject& rhs){
		 data=rhs.data;
		 coord=rhs.coord;
		 _tx=rhs._tx; _ty=rhs._ty; _tz=rhs._tz;
		 _sx=rhs._sx; _sy=rhs._sy; _sz=rhs._sz;
		 _fi=rhs._fi; _theta=rhs._theta;
	 }*/
 } gameManager;






float Refx=0.0f, Refy=0.0f, Refz=1.0f;
float Vx=0.0;
glm::mat4 view;


float PI=3.141592;

// elemente pentru matricea de vizualizare
float Obsx=75.0, Obsy=75.0, Obsz=-425.0f;

	 baundingBox::baundingBox(){};
	 int baundingBox::Add(rect3 x){
		C.push_back(x);
		return C.size();
	 }
	 bool baundingBox::insideCollisionZone(int a){
		 if(C[a].mx<=Obsx && Obsx<=C[a].MX && C[a].my<=Obsy && Obsy<=C[a].MY && C[a].mz<=Obsz && Obsz<=C[a].MZ )return true;return false;}
	 vector<int> baundingBox::getLocation(){vector<int> ret;for(int a=0;a<C.size();a++)if(insideCollisionZone(a))ret.push_back(a);return ret;}
	 
	 bool baundingBox::closeToCollisionZone(int a,float thresh){if( abs(Obsx-(C[a].mx+C[a].MX)/2.0)<=thresh && abs(Obsy-(C[a].my+C[a].MY)/2.0)<=thresh && abs(Obsz-(C[a].mz+C[a].MZ)/2.0)<=thresh )return true;return false;}
	 vector<int> baundingBox::undeSuntCloseEnough(float veigar){vector<int> ret;for(int a=0;a<C.size();a++)if(closeToCollisionZone(a,veigar))ret.push_back(a);return ret;}
	 
	 void baundingBox::trigger(int x){
		 C[x].calBack(C[x].target,C[x]);
	 }

	 gameObject::gameObject(GLfloat* vf, int nvf, GLuint* cd, int ncd, string eu){
		 data.clear();
		 data=vector<GLfloat>();
		 orig_vf.clear();
		 orig_vf=vector<GLfloat>();
		 coord.clear();
		 coord=vector<GLuint>();
		 for(int i=0;i<nvf;i++){
			 data.push_back(vf[i]);
			 orig_vf.push_back(vf[i]);
		 }
		 for(int i=0;i<ncd;i++)
			 coord.push_back(cd[i]);
		 _sx=_sy=_sz=1;
		 _tx=_ty=_tz=0;
		 _fi=_theta=0;
		 nume=eu;
		 real_name=eu;
	 }
	 gameObject::gameObject(){
		 _sx=_sy=_sz=1;
		 _tx=_ty=_tz=0;
		 _fi=_theta=0;
		 nume="gandi";
		 real_name="gandi";
	 }
	 void gameObject::update_base(){

		 //data=orig_vf;
		 //data=vector<GLfloat>();

		 //for(int a=0;a<data.size();a++)
			// if(data[a]!=orig_vf[a])
				// printf("!!!!!\n");
			//printf("%d si %d!!\n",data.size(),orig_vf.size());
			 //data.push_back(orig_vf[a]);
		 data.clear();
		 data=vector<GLfloat>();
		 for(int a=0;a<orig_vf.size();a++){
			 data.push_back(orig_vf[a]);
		 }
		  for(int i=0;i<colliders.C.size();i++)
			  colliders.C[i].mx=colliders.C[i].omx,
			  colliders.C[i].my=colliders.C[i].omy,
			  colliders.C[i].mz=colliders.C[i].omz,
			  colliders.C[i].MX=colliders.C[i].OMX,
			  colliders.C[i].MY=colliders.C[i].OMY,
			  colliders.C[i].MZ=colliders.C[i].OMZ;
		 for(int i=0;i<colliders.C.size();i++){
			 colliders.C[i].mx*=_sx;
			 colliders.C[i].my*=_sy;
			 colliders.C[i].mz*=_sz;
			 colliders.C[i].MX*=_sx;
			 colliders.C[i].MY*=_sy;
			 colliders.C[i].MZ*=_sz;
			 if(abs(_fi)>0.01)
			 {
				 GLfloat ox=colliders.C[i].mx;
				 GLfloat oy=colliders.C[i].my;
				 GLfloat oz=colliders.C[i].mz;
				 colliders.C[i].my=oy*cos(_theta) - ox*sin(_theta);
				 colliders.C[i].mx=oy*sin(_theta) + ox*cos(_theta); 
				 ox=colliders.C[i].mx;
				 colliders.C[i].mx=ox*cos(_fi) - oz*sin(_fi);
				 colliders.C[i].mz=ox*sin(_fi) + oz*cos(_fi);
			 }
			 if(abs(_fi)>0.01)
			 {
				 GLfloat ox=colliders.C[i].MX;
				 GLfloat oy=colliders.C[i].MY;
				 GLfloat oz=colliders.C[i].MZ;
				 colliders.C[i].MY=oy*cos(_theta) - ox*sin(_theta);
				 colliders.C[i].MX=oy*sin(_theta) + ox*cos(_theta); 
				 ox=colliders.C[i].MX;
				 colliders.C[i].MX=ox*cos(_fi) - oz*sin(_fi);
				 colliders.C[i].MZ=ox*sin(_fi) + oz*cos(_fi);
			 }

			 colliders.C[i].mx+=_tx;
			 colliders.C[i].my+=_ty;
			 colliders.C[i].mz+=_tz;
			 colliders.C[i].MX+=_tx;
			 colliders.C[i].MY+=_ty;
			 colliders.C[i].MZ+=_tz;
		 }
		 for(int i=0;i<data.size()/9;i++)
			 data[9*i+0]*=_sx,
			 data[9*i+1]*=_sy,
			 data[9*i+2]*=_sz;
		 for(int i=0;i<data.size()/9;i++){
			 if(abs(_fi)>0.01)
			 {
				 GLfloat ox=data[9*i+0];
				 GLfloat oy=data[9*i+1];
				 GLfloat oz=data[9*i+2];
				 data[9*i+1]=oy*cos(_theta) - ox*sin(_theta);
				 data[9*i+0]=oy*sin(_theta) + ox*cos(_theta); 
				 ox=data[9*i+0];
				 data[9*i+0]=ox*cos(_fi) - oz*sin(_fi);
				 data[9*i+2]=ox*sin(_fi) + oz*cos(_fi);
			 }
			 if(abs(_fi)>0.01)
			 {
				 GLfloat ox=data[9*i+4];
				 GLfloat oy=data[9*i+5];
				 GLfloat oz=data[9*i+6];
				 data[9*i+5]=oy*cos(_theta) - ox*sin(_theta);
				 data[9*i+4]=oy*sin(_theta) + ox*cos(_theta); 
				 ox=data[9*i+4];
				 data[9*i+4]=ox*cos(_fi) - oz*sin(_fi);
				 data[9*i+6]=ox*sin(_fi) + oz*cos(_fi);
			 }
		 }
		 for(int i=0;i<data.size()/9;i++)
			 data[9*i+0]+=_tx,
			 data[9*i+1]+=_ty,
			 data[9*i+2]+=_tz;
		 nume=real_name;
		 si_dupa();
	 }
	 void gameObject::update(){
		 update_base();
	 }
	 rect3::rect3(float _mx,float _MX,float _my,float _MY,float _mz,float _MZ,void (*f)(gameObject*,rect3),gameObject* care)
	 {
		 mx=_mx;my=_my;mz=_mz;
		 MX=_MX;MY=_MY;MZ=_MZ;
		 omx=_mx;omy=_my;omz=_mz;
		 OMX=_MX;OMY=_MY;OMZ=_MZ;
		 calBack=f;
		 target=care;
	 }

int codCol;
// matrice utilizate
glm::mat4 myMatrix, matrRot; 
// elemente pentru matricea de proiectie
float width=800, height=600, xwmin=-800.f, xwmax=800, ywmin=-600, ywmax=600, znear=1, zfar=-1, fov=45;
glm::mat4 projection;

class p_mesh{
public:
	GLuint index;
	GLfloat x;
	GLfloat y;
	p_mesh(){};
	p_mesh(GLuint _i, GLfloat _x, GLfloat _y){
		index=_i; x=_x; y=_y;
	}
};
class t_mesh{
public:
	p_mesh A;
	p_mesh B;
	p_mesh C;
	t_mesh():A(0,0,0),B(0,0,0),C(0,0,0){};
	t_mesh(GLuint _iA, GLfloat _xA, GLfloat _yA,
		   GLuint _iB, GLfloat _xB, GLfloat _yB,
		   GLuint _iC, GLfloat _xC, GLfloat _yC, bool swapp=false):
			A(_iA,_xA,_yA),
			B(_iB,_xB,_yB),
			C(_iC,_xC,_yC){
				if(swapp){
					swap(B.index,C.index);
					swap(B.x,C.x);
					swap(B.y,C.y);
				}
			};
	/*t_mesh& operator=(t_mesh rhs){
		A=rhs.A;
		B=rhs.B;
		C=rhs.C;
		return *this;
	}*/
};
class mesh{
public:
	GLfloat* vf; int nvf;
	GLuint* cd; int ncd;
	mesh(glm::vec3* data, int nvarfuri,t_mesh* tri, int ntri, bool tx_units=true){
		vf=new GLfloat[9*3*ntri];
		cd=new GLuint[3*ntri];
		nvf=9*3*ntri;
		ncd=3*ntri;
		for(int i=0;i<ntri;i++){
			cd[3*i+0]=3*i+0;
			cd[3*i+1]=3*i+1;
			cd[3*i+2]=3*i+2;

			glm::vec3 AB = data[tri[i].A.index]-data[tri[i].B.index];
			glm::vec3 AC = data[tri[i].A.index]-data[tri[i].C.index];
			glm::vec3 pv = glm::cross(AB,AC);
			pv=glm::normalize(pv);

			vf[9*3*i + 9*0 + 0]=data[tri[i].A.index].x;
			vf[9*3*i + 9*0 + 1]=data[tri[i].A.index].y;
			vf[9*3*i + 9*0 + 2]=data[tri[i].A.index].z;
			vf[9*3*i + 9*0 + 3]=1.0f;
			vf[9*3*i + 9*0 + 4]=pv.x;
			vf[9*3*i + 9*0 + 5]=pv.y;
			vf[9*3*i + 9*0 + 6]=pv.z;
			vf[9*3*i + 9*0 + 7]=tri[i].A.x *(tx_units?tex_sz:1);
			vf[9*3*i + 9*0 + 8]=tri[i].A.y *(tx_units?tex_sz:1);

			//if(tx_units)cout<<vf[9*3+i + 9*0 + 7]<<"!!!!\n";

			vf[9*3*i + 9*1 + 0]=data[tri[i].B.index].x;
			vf[9*3*i + 9*1 + 1]=data[tri[i].B.index].y;
			vf[9*3*i + 9*1 + 2]=data[tri[i].B.index].z;
			vf[9*3*i + 9*1 + 3]=1.0f;
			vf[9*3*i + 9*1 + 4]=pv.x;
			vf[9*3*i + 9*1 + 5]=pv.y;
			vf[9*3*i + 9*1 + 6]=pv.z;
			vf[9*3*i + 9*1 + 7]=tri[i].B.x * (tx_units?tex_sz:1);
			vf[9*3*i + 9*1 + 8]=tri[i].B.y * (tx_units?tex_sz:1);
			
			vf[9*3*i + 9*2 + 0]=data[tri[i].C.index].x;
			vf[9*3*i + 9*2 + 1]=data[tri[i].C.index].y;
			vf[9*3*i + 9*2 + 2]=data[tri[i].C.index].z;
			vf[9*3*i + 9*2 + 3]=1.0f;
			vf[9*3*i + 9*2 + 4]=pv.x;
			vf[9*3*i + 9*2 + 5]=pv.y;
			vf[9*3*i + 9*2 + 6]=pv.z;
			vf[9*3*i + 9*2 + 7]=tri[i].C.x *(tx_units?tex_sz:1);
			vf[9*3*i + 9*2 + 8]=tri[i].C.y * (tx_units?tex_sz:1);
		}
	}
	~mesh(){
		delete[] vf;
		delete[] cd;
	}
};

 class slotMachine:public gameObject{
 public:
	 GLfloat _ecrx;
	 GLfloat _ecry;
	 GLfloat _ecrw;
	 GLfloat _ecrh;
	 slotMachine(){
	 };
	 slotMachine(GLfloat* vf, int nvf, GLuint* cd, int ncd,string eu):gameObject(vf,nvf,cd,ncd,eu){
		 
	 }
	 gameObject* deepCopy(){
		 slotMachine* trb=new slotMachine();
		 trb->nume=nume;
		 trb->real_name=real_name;
		 trb->_ecrx=_ecrx;
		 trb->_ecry=_ecry;
		 trb->_ecrw=_ecrw;
		 trb->_ecrh=_ecrh;


		 trb->orig_vf=orig_vf;
		 trb->data=data;
		 trb->coord=coord;
		  trb->_tx=_tx;trb->_ty=_ty;trb->_tz=_tz;
		  trb->_sx=_sx;trb->_sy=_sy;trb->_sz=_sz;
		  trb->_fi=_fi;trb->_theta=_theta;
		  trb->colliders = baundingBox();
		  trb->colliders.C = vector<rect3>();
		  for(int i=0;i<colliders.C.size();i++){
			  trb->colliders.C.push_back(colliders.C[i]);
			  //if(colliders.C[colliders.C.size()-1].target==&rhs){
				  trb->colliders.C[trb->colliders.C.size()-1].target=trb;
			  //}
		  }
		  return trb;	
	 }
	 void update_pacanea(){
		 if(_ecrw <=0 || _ecrh <= 0)return;
		 //printf("Aici");
		 //system("PAUSE");
		 //return;
		 orig_vf[60 * 9 + 7] = _ecrx;
		 orig_vf[60 * 9 + 8] = _ecry;

		 orig_vf[62 * 9 + 7] = _ecrx+_ecrw;
		 orig_vf[62 * 9 + 8] = _ecry+_ecrh;

		 orig_vf[61 * 9 + 7] = _ecrx;
		 orig_vf[61 * 9 + 8] = _ecry+_ecrh;

		 orig_vf[63 * 9 + 7] = _ecrx;
		 orig_vf[63 * 9 + 8] = _ecry;

		 orig_vf[64 * 9 + 7] = _ecrx+_ecrw;
		 orig_vf[64 * 9 + 8] = _ecry+_ecrh;

		 orig_vf[65 * 9 + 7] = _ecrx+_ecrw;
		 orig_vf[65 * 9 + 8] = _ecry; 

	 }
	 void update(){
		 update_pacanea();
		 update_base();
	 }
 };
#include <fstream>
 map<int,gameObject*> tempy;
 
	 int lista_trebi::numVertices(){
		 int res=0; for(int i=0;i<children.size();i++)res+=children[i]->orig_vf.size(); return res/9;
	 }
	 int lista_trebi::numTriangles(){
		 int res=0; for(int i=0;i<children.size();i++)res+=children[i]->coord.size(); return res/3;
	 }

	 void lista_trebi::UpdateBuffer(){
		 
		 //printf("!");
		multi.resize(numVertices()*9); 
		multe.resize(numTriangles()*3);
		
		//printf("%d %d !!!\n",numVertices(),numTriangles());
		//printf("%d %d\n",numVertices(),numTriangles());
		for(int last_v=0,last_i=0,i=0;i<children.size();i++){
			//printf("%d %d",children[i]->data.size(),children[i]->coord.size());
			//if(children[i]->ID==20)
			//cout<<children.size()<<" "<<children[i]->ID<<" "<<i<<endl;

			for(int j=0;j<children[i]->data.size();j++){
				multi[last_v+j]=children[i]->data[j];
				if(j%9==7)multi[last_v+j]/=img_width;
				if(j%9==8)multi[last_v+j]=1-(multi[last_v+j]/img_height);
				
				//printf("%f, ",multi[last_v+j]);
				//if(j%9==8)printf("\n");
			}
			for(int j=0;j<children[i]->coord.size();j++){
				multe[last_i+j]=children[i]->coord[j]+last_v/9;
				//printf("%d, ",multe[last_i+j]);
				//if(j%3==2)printf("\n");
			}
			last_v+=children[i]->data.size();
			last_i+=children[i]->coord.size();
		}
	 }

	 void lista_trebi::CreateVBO(){
		// se creeaza un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO
		glGenVertexArrays(1, &VaoId);
		// se creeaza un buffer nou (atribute)
		glGenBuffers(1, &VboId);
		// se creeaza un buffer nou (indici)
		glGenBuffers(1, &EboId);
		// legarea VAO 
		glBindVertexArray(VaoId);

		// legarea buffer-ului "Array"
		glBindBuffer(GL_ARRAY_BUFFER, VboId);
		// punctele sunt "copiate" in bufferul curent
		glBufferData(GL_ARRAY_BUFFER, numVertices()*9*sizeof(GLfloat), multi.data(), GL_DYNAMIC_DRAW);

		

		// legarea buffer-ului "Element" (indicii)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
		// indicii sunt "copiati" in bufferul curent
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numTriangles()*3*sizeof(GLuint), multe.data(), GL_DYNAMIC_DRAW);
  
		// se activeaza lucrul cu atribute; atributul 0 = pozitie
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),(GLvoid*)0);

		// se activeaza lucrul cu atribute; atributul 1 = culoare
		
		
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),(GLvoid*)(4 * sizeof(GLfloat)));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),(GLvoid*)(4 * sizeof(GLfloat)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),(GLvoid*)(7 * sizeof(GLfloat)));
		
	 }
 
compositeGameObject:: compositeGameObject(){
		_sx=_sy=_sz=1;
	}
void compositeGameObject::update_lista(){
	colliders=baundingBox();
	orig_vf.clear();
	orig_vf=vector<GLfloat>();
	data.clear();
	data=vector<GLfloat>();
	coord.clear();
	coord=vector<GLuint>();
		propaga(real_name);
	for(int i=0;i<proprii.C.size();i++)
		colliders.C.push_back(proprii.C[i]);
	for(int i=0,last_v=0;i<children.size();i++){
		for(int j=0;j<children[i]->colliders.C.size();j++){
			colliders.C.push_back(children[i]->colliders.C[j]),
			colliders.C[colliders.C.size()-1].omx=colliders.C[colliders.C.size()-1].mx,
			colliders.C[colliders.C.size()-1].omy=colliders.C[colliders.C.size()-1].my,
			colliders.C[colliders.C.size()-1].omz=colliders.C[colliders.C.size()-1].mz,
			colliders.C[colliders.C.size()-1].OMX=colliders.C[colliders.C.size()-1].MX,
			colliders.C[colliders.C.size()-1].OMY=colliders.C[colliders.C.size()-1].MY,
			colliders.C[colliders.C.size()-1].OMZ=colliders.C[colliders.C.size()-1].MZ;
		}
		for(int j=0;j<children[i]->data.size();j++){
			data.push_back(children[i]->data[j]);
			orig_vf.push_back(children[i]->data[j]);
		}
		for(int j=0;j<children[i]->coord.size();j++)
			coord.push_back(children[i]->coord[j]+last_v);
		last_v+=children[i]->data.size()/9;
	}
}
void compositeGameObject::update(){
		 
	for(int i=0;i<children.size();i++)
		children[i]->update();
	update_lista();
	update_base();
}
 rect3 treabaToCutie(gameObject *x,void (*calBack)(gameObject*,rect3))
 {
	 float mx,MX,my,MY,mz,MZ;
	 for(int a=0;a<x->orig_vf.size()/9;a++)
	 {
		 if(a==0)
			 mx=MX=x->orig_vf[9*a+0],
			 my=MY=x->orig_vf[9*a+1],
			 mz=MZ=x->orig_vf[9*a+2];
		 else
			 mx=min(mx,x->orig_vf[9*a+0]),MX=max(MX,x->orig_vf[9*a+0]),
			 my=min(my,x->orig_vf[9*a+1]),MY=max(MY,x->orig_vf[9*a+1]),
			 mz=min(mz,x->orig_vf[9*a+2]),MZ=max(MZ,x->orig_vf[9*a+2]);
	 }
	 return rect3(mx,MX,my,MY,mz,MZ,calBack,x);
 }
void displayMatrix ( )
{
	return;
	for (int ii = 0; ii < 4; ii++)
	{
		for (int jj = 0; jj < 4; jj++)
		cout <<  myMatrix[ii][jj] << "  " ;
		cout << endl;
	};
};
double max(double a,double b){if(a>b)return a;return b;}
double min(double a,double b){if(a<b)return a;return b;}

time_t time_catsin;
void processNormalKeys(unsigned char key, int x, int y)
{
	if(clock()<time_catsin)return;
	double dx=0,dz=0;

	switch (key) {
		
		case 'w' :
			dz += 20;
			break;
		case 's' :
			dz -= 20;
			break;

		case 'a' :
			dx += 20;
			break;
		case 'd' :
			dx -= 20;
			break;


		case 'l' :
			Vx += 0.1;
			break;
		case 'r' :
			Vx -= 0.1;
			break;
		case '+' :
			//znear += 10;
			//zfar += 10;
			Obsy+=10;
			break;
		case '-' :
			//znear -= 10;
			//zfar -= 10;
			Obsy-=10;
			break;

	}
	double ax=Obsx,az=Obsz;
	Obsz+=Refz*dz - Refx*dx;
	Obsx+=Refx*dz + Refz*dx;
	if(permise.getLocation().size()==0)
	{
		double bx=Obsx, bz=Obsz;
		Obsx=ax;
		if(permise.getLocation().size()==0){
			Obsx=bx;
			Obsz=az;
			if(permise.getLocation().size()==0){
				Obsx=ax;
			}
		}
	}
if (key == 27)
exit(0);
}
double fi=0.0;
double theta=0.0;
void processSpecialKeys(int key, int xx, int yy) {
	
	if(clock()<time_catsin)return;
	double stepH=0.1;
	double stepV=0.1;
	switch (key) {
		case GLUT_KEY_LEFT :
			fi+=0.1;
			break;
		case GLUT_KEY_RIGHT :
			fi-=0.1;
			break;
		case GLUT_KEY_UP :
			if(theta<PI/2-0.1)
				theta+=0.1;
			break;
		case GLUT_KEY_DOWN :
			if(theta>-PI/2+0.1)
				theta-=0.1;
			break;
	}
	Refx=cos(theta)*sin(fi);
	Refy=sin(theta);
	Refz=cos(theta)*cos(fi);
	if(fi>2*PI)fi-=2*PI;
	//if(theta>2*PI)theta-=2*PI;
	if(fi<-2*PI)fi+=2*PI;
	//if(theta>2*PI)theta-=2*PI;
}

void DestroyVBO(void)
{
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
 
  glDeleteBuffers(1, &VboId);
  glDeleteBuffers(1, &EboId);

  glBindVertexArray(0);
  glDeleteVertexArrays(1, &VaoId);
  glDeleteVertexArrays(1, &EboId);
   
}

int imgwidth, imgheight;
    unsigned char* image;
void LoadTexture(void)
{
	
	glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	// Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gameManager.img_height=imgheight;
	gameManager.img_width=imgwidth;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgwidth, imgheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

} 
 
void CreateShaders(void)
{
  ProgramId=LoadShaders("Shader.vert", "Shader.frag");//era 11_03 cand mergea
  glUseProgram(ProgramId);
}
 
void DestroyShaders(void)
{
  glDeleteProgram(ProgramId);
}
void Initialize(void)
{
    myMatrix = glm::mat4(1.0f);
 
	matrRot=glm::rotate(glm::mat4(1.0f), PI/8, glm::vec3(0.0, 0.0, 1.0));
  
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // culoarea de fond a ecranului
}
#include <Windows.h>
#include <time.h>

bool scanez_pacanele=true;
bool mid_flight=false;
bool explozie=false;
clock_t flight_start;
void mesagerie(gameObject* care, rect3 box){
	slotMachine* slot=dynamic_cast<slotMachine*>(care);

	if(slot!=NULL){
		slot->_ecry=tex_sz;
		if(strstr(care->nume.c_str(),"Camera_1")!=NULL)
		{
			bool piton=1;
			for(int a=0;a<toggledSlots.size();a++)
				if(strcmp(toggledSlots[a].c_str(),care->nume.c_str())==0){
					piton=0;break;}
			if(piton)
				toggledSlots.push_back(care->nume),
				cout<<toggledSlots.size();
			if(toggledSlots.size()>=5 && scanez_pacanele)//todo pune 48
			{
				scanez_pacanele=false;
				cout<<"FECILITARI\n";
				for(int a=0;a<toggledSlots.size();a++){
					cout<<toggledSlots[a].c_str()<<endl;
				}
				mid_flight=true;
				flight_start=clock();
			}
		}
	}
}
int a_fost;
void moveFunction()
{
	static bool CE=0;
	static clock_t last_frame;
	gameManager.findObject("_root.Camera_2.Sfera")[0]->_fi+=0.1;	
	gameManager.findObject("_root.Camera_2.bAnanas")[0]->_fi-=0.1;	
	if(time_catsin>clock()){
		double procc = 1 - (time_catsin - clock())*1.0/5000;
		//printf("%f",procc);
		double proc;
		if(procc < 0.25) 
			proc = 1- 3*procc;
		else if(procc >0.75)
			proc = 1 - 3*(1-procc);
		else
			proc=0;
		fi = ofi * proc + 1.6 *(1-proc);
		theta = saunuofi * proc + 0.4 * (1-proc);
		Refx=cos(theta)*sin(fi);
		Refy=sin(theta);
		Refz=cos(theta)*cos(fi);
		if(fi>2*PI)fi-=2*PI;
		//if(theta>2*PI)theta-=2*PI;
		if(fi<-2*PI)fi+=2*PI;
	}
	if(mid_flight){
		clock_t acum=clock();
		if(acum-flight_start>=10000){
			mid_flight=false;
			explozie=true;
			flight_start=acum;
			gameManager.findObject("_root.Camera_1.Pacanea_4_5")[0]->_ty=-100;
			gameManager.findObject("_root.Camera_1.Pacanea_4_6")[0]->_ty=-100;
			gameObject* explozia=tempy[3]->deepCopy();
			explozia->_ty=60;
			explozia->_sy=60;
			explozia->_sx=50;
			explozia->_sz=70;
			explozia->_tx=498;
			explozia->_tz=15;
			explozia->_fi=PI/2;
			explozia->real_name="Eusebiu";
			explozia->nume="Eusebiu";
			explozia->update();
			vector<gameObject*> candidati=gameManager.findObject("_root.Camera_1");
			for(int i=0;i<candidati.size();i++)
				 if(!strcmp(candidati[i]->nume.c_str(),"_root.Camera_1")){
					 dynamic_cast<compositeGameObject*>(candidati[i])->children.push_back(explozia);
					 gameManager.update();
					 break;
				 }
			gameManager.update();
			CE=1;
			return;

		};
		//printf("%l %l\n",acum,acum-flight_start);
		vector<gameObject*> goingDown;
		
		vector<gameObject*> query=gameManager.findObject("_root.Camera_1.Pacanea_4_5");
		for(int j=0;j<query.size();j++){
			goingDown.push_back(query[j]);
		}
		query.clear();
		query=gameManager.findObject("_root.Camera_1.Pacanea_4_6");
		for(int j=0;j<query.size();j++){
			goingDown.push_back(query[j]);
		}
		query.clear();
		cout<<goingDown.size();
		for(int a=0;a<goingDown.size();a++){
			printf("twas me all along\n");
			goingDown[a]->_ty=50-(acum-flight_start)*100.0f/10000;
			if(CE)system("PAUSE");
		}
		gameManager.update();
	}
	static int last_pas=0;
	if(explozie){
		clock_t acum=clock();
		if(acum-flight_start>=3000){
			explozie=false;
			rect3 c1=permise.C[0];
			rect3 c3=c1;
			c3.mx=300;
			c3.MX=650;
			c3.mz=-50;
			c3.MZ=50;
			permise.C.push_back(c3);
			gameManager.findObject("_root.Camera_2.Pacanea_2_6")[0]->_ty=-100;
			gameManager.findObject("_root.Camera_2.Pacanea_2_5")[0]->_ty=-100;
			//gameManager.findObject("_root.Camera_1.Pacanea_2_6")[0]->_ty=-100;
			//gameManager.findObject("_root.Camera_1.Pacanea_2_5")[0]->_ty=-100;
			gameObject* Eusebiu2 = gameManager.findObject("_root.Camera_1.Eusebiu")[0]->deepCopy();
			Eusebiu2->real_name="Eusebiu_joaca_basket";
			Eusebiu2->_tx=-Eusebiu2->_tx;
			dynamic_cast<compositeGameObject*>(gameManager.findObject("_root.Camera_2")[0])->children.push_back(Eusebiu2);
		}
		int n_frames=25;
		int pas=(acum-flight_start)*n_frames/3000;
		if(pas>=n_frames)pas=n_frames-1;

		int n_i,n_j;
		n_i=pas/5;
		n_j=pas%5;
		double px=160+64*n_j;
		double py=128+64*n_i;

		//if(pas>last_pas){
			gameObject* explozia=gameManager.findObject("Eusebiu")[0];
			explozia->orig_vf[9*0 + 7] = px;
			explozia->orig_vf[9*0 + 8] = py+64;

			explozia->orig_vf[9*1 + 7] = px;
			explozia->orig_vf[9*1 + 8] = py;

			explozia->orig_vf[9*2 + 7] = px+64;
			explozia->orig_vf[9*2 + 8] = py;

			explozia->orig_vf[9*3 + 7] = px;
			explozia->orig_vf[9*3 + 8] = py+64;

			explozia->orig_vf[9*4 + 7] = px+64;
			explozia->orig_vf[9*4 + 8] = py;
			
			explozia->orig_vf[9*5 + 7] = px+64;
			explozia->orig_vf[9*5 + 8] = py+64;
			gameManager.update();
		//}
		last_pas=pas;
	}
	last_frame=clock();

	if(1)
	{
		vector<int> toti=gameManager.colliders.getLocation();
	if(0)
		for(int i=0;i<gameManager.colliders.C.size();i++){
			printf("Box:\n\t[%f %f %f]\n\t[%f %f %f]\n\t%s\n\t%s\n",
				gameManager.colliders.C[i].mx,
				gameManager.colliders.C[i].my,
				gameManager.colliders.C[i].mz,
				gameManager.colliders.C[i].MX,
				gameManager.colliders.C[i].MY,
				gameManager.colliders.C[i].MZ,
				gameManager.colliders.C[i].target->nume.c_str(),
				gameManager.colliders.C[i].target->real_name.c_str());
		}
		if(1)
		{
			if(a_fost==1 && clock()>=time_catsin)
			{
				fi=ofi;
				theta=saunuofi;
				
				Refx=cos(theta)*sin(fi);
				Refy=sin(theta);
				Refz=cos(theta)*cos(fi);
				time_catsin=0;
				a_fost=123;
			}
			if(permise.insideCollisionZone(1) && a_fost==0)//nu scimba ca e catsinnnnnnnnnnnnnnn!!!!!!!!!!!!!!!!!!!!!!!
			{
				
				a_fost++;
				ofi=fi;
				saunuofi=theta;
				/*
				fi=1.6;
				theta=0.3;

				Refx=cos(theta)*sin(fi);
				Refy=sin(theta);
				Refz=cos(theta)*cos(fi);*/
				time_catsin=clock()+5000;
			}


			/*
			cout<<"Deci eu sunt la: "<<Obsx<<" "<<Obsy<<" "<<Obsz<<" si ma uit la: "<<fi<<" "<<theta<<endl;
			for(int i=0;i<permise.C.size();i++)
			printf("Box:\n\t[%f %f %f]\n\t[%f %f %f]\n",
				permise.C[i].mx,
				permise.C[i].my,
				permise.C[i].mz,
				permise.C[i].MX,
				permise.C[i].MY,
				permise.C[i].MZ);
				*/

		}
		static bool clos=true;
		gameManager.colliders.triggerAllClose(50.0f);
		gameManager.update();
	}
}
void RenderFunction(void)
{
	moveFunction();
	//int t=clock();
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
    
   // se schimba pozitia observatorului
	glm::vec3 Obs = glm::vec3 (Obsx, Obsy, Obsz); 

	// pozitia punctului de referinta

	//Refx=Obsx; Refy=Obsy;
	glm::vec3 PctRef = glm::vec3(Refx+Obsx, Refy+Obsy, Refz+Obsz); 

	// verticala din planul de vizualizare 
	
	glm::vec3 Vert =  glm::vec3(Vx, 1.0f, 0.0f);

  	view = glm::lookAt(Obs, PctRef, Vert);
	 	 
	 myMatrix=view;
     displayMatrix();

//    projection = glm::ortho(xwmin, xwmax, ywmin, ywmax, znear, zfar);
 //  projection = glm::frustum(xwmin, xwmax, ywmin, ywmax, 80.f, -80.f);
  projection = glm::perspective(fov, GLfloat(width)/GLfloat(height), znear, zfar);
  myMatrix = glm::mat4(1.0f);
 
  
  LoadTexture( );
  gameManager.UpdateBuffer();
  gameManager.CreateVBO();
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

 // myMatrix= matrRot;
  CreateShaders();
 // variabile uniforme pentru shaderul de varfuri
  myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
  glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE,  &myMatrix[0][0]);
  viewLocation = glGetUniformLocation(ProgramId, "view");
  glUniformMatrix4fv(viewLocation, 1, GL_FALSE,  &view[0][0]);
  projLocation = glGetUniformLocation(ProgramId, "projection");
  glUniformMatrix4fv(projLocation, 1, GL_FALSE,  &projection[0][0]);
 
  GLint depthNearLoc = glGetUniformLocation(ProgramId, "gl_DepthRange.near");
  GLint depthFarLoc = glGetUniformLocation(ProgramId, "gl_DepthRange.far");
  GLint depthDiffLoc = glGetUniformLocation(ProgramId, "gl_DepthRange.diff");

  glUniform1f (depthNearLoc, -1.0);
  glUniform1f (depthFarLoc , 2.0);
  glUniform1f (depthDiffLoc, 1.0);

   glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

     GLint objectColorLoc = glGetUniformLocation(ProgramId, "objectColor");
  GLint lightColorLoc  = glGetUniformLocation(ProgramId, "lightColor");
  GLint lightPosLoc    = glGetUniformLocation(ProgramId, "lightPos");
  GLint viewPosLoc     = glGetUniformLocation(ProgramId, "viewPos");
  
  //glUniform3f(objectColorLoc, 0.3f, 0.3f, 0.3f);
  //glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
  
  static double c1=0,c2=0,c3=0,c4=0,c5=0,c6=0;
  static double x1=0,x2=0,x3=0,x4=0,x5=0,x6=0;

  if(fabs(c1-x1)<=0.05 && fabs(c2-x2)<=0.05 && fabs(c3-x3)<=0.05 && fabs(c4-x4)<=0.05 && fabs(c5-x5)<=0.05 && fabs(c6-x6)<=0.05)
  {
	  x1=rand()%100/200.0f;
	  x2=rand()%100/200.0f;
	  x3=rand()%100/200.0f;
	  x4=rand()%100/200.0f;
	  x5=rand()%100/200.0f;
	  x6=rand()%100/200.0f;
  } else {
	  c1=c1+0.3*(x1-c1);
	  c2=c2+0.3*(x2-c2);
	  c3=c3+0.3*(x3-c3);
	  c4=c4+0.3*(x4-c4);
	  c5=c5+0.3*(x5-c5);
	  c6=c6+0.3*(x6-c6);
  }  
  glUniform3f(objectColorLoc, c1, c2, c3);
  glUniform3f(lightColorLoc, c4, c5, c6);

	glUniform3f(lightPosLoc, Obsx,-20,Obsz );
	glUniform3f(viewPosLoc, Obsx,20000,Obsz);
  
	//glUniform3f(lightPosLoc, 50,50,50 );
	//glUniform3f(viewPosLoc, 50,50,50);
   glDrawElements(GL_TRIANGLES, gameManager.numTriangles()*3,  GL_UNSIGNED_INT, 0);
  // Eliberare memorie si realocare resurse
   
   DestroyVBO ( );
   DestroyShaders ( );
  


  glutSwapBuffers();
  glFlush ( );
  //cout<<"Tim: "<<clock()-t<<endl;
}
void Cleanup(void)
{
  DestroyShaders();
  DestroyVBO();
	SOIL_free_image_data(image);
}
int main(int argc, char* argv[])
{	
	{
		glm::vec3 Varfe[]={
			glm::vec3(-25,-50,-25), //1  0
			glm::vec3(-25,-50, 25), //2  1
			glm::vec3(-25,  0, 25), //3  2
			glm::vec3(-25,  0,-25), //4  3
			glm::vec3(-25, 50, 25), //5

			glm::vec3(-25, 50,-25), //6
			glm::vec3(-15, 50, 25), //7
			glm::vec3(-15, 50,-25), //8
			glm::vec3(  0,  0, 25), //9
			glm::vec3(  0,  0,-25), //10

			glm::vec3( 25,  0, 25), //11
			glm::vec3( 25,  0,-25), //12
			glm::vec3( 25,-50, 25), //13
			glm::vec3( 25,-50,-25), //14
		};
		t_mesh Tri[]={
			t_mesh( 0, 5, 3,  1, 6, 3,  2, 6, 2), //1 2 3
		    t_mesh( 0, 5, 3,  2, 6, 2,  3, 5, 2), //1 3 4
			t_mesh( 3, 5, 2,  2, 6, 2,  4, 6, 1), //4 3 5
			t_mesh( 3, 5, 2,  4, 6, 1,  5, 5, 1), //4 5 6
			t_mesh( 5, 5, 1,  4, 6, 1,  6, 6,.8), //6 5 7
			t_mesh( 5, 5, 1,  6, 6,.8,  7, 5,.8), //6 7 8
			t_mesh( 3, 5, 2,  5, 5, 1,  7,4.8,1), //4 6 8
			t_mesh( 3, 5, 2,  7,4.8,1,  9,4.5,2), //4 8 10
			t_mesh( 3, 5, 2, 11, 4, 2, 13, 4, 3,true), //4 12 14
			t_mesh( 0, 5, 3, 13, 4, 3,  3, 5, 2), //1 14 4
			t_mesh( 0, 5, 3,  1, 6, 3, 13, 5, 4), //1 2 14
			t_mesh( 1, 6, 3, 13, 5, 4, 12, 6, 4), //2 14 13
			t_mesh( 1, 6, 3, 12, 7, 3, 10, 7, 2), //2 13 11
			t_mesh( 1, 6, 3, 10, 7, 2,  2, 6, 2), //2 11 3
			t_mesh( 2, 6, 2,  6,6.2,1,  8,6.5,2), //3  7 9
			t_mesh( 2, 6, 2,  6,6.2,1,  4, 6, 1), //3  7 5
			t_mesh(12, 7, 3, 11, 8, 2, 13, 8, 3,true), //13 12 14
			t_mesh(12, 7, 3, 11, 8, 2, 10, 7, 2), //13 12 11
			t_mesh(10, 7, 2,  9,8,1.5, 11, 8, 2,true), 
			t_mesh(10, 7, 2,  9,8,1.5,  8,7,1.5),
			t_mesh( 6, 0, 0,  9, 1, 1,  8, 0, 1,true),
			t_mesh( 6, 0, 0,  9, 1, 1,  7, 1, 0)
		};
		mesh meshy(Varfe,14,Tri,22);
		slotMachine* templ=new slotMachine(meshy.vf,meshy.nvf,meshy.cd,meshy.ncd,"Pacanea Template");
		templ->_ecrx=0;
		templ->_ecry=0;
		templ->_ecrw=tex_sz;
		templ->_ecrh=tex_sz;
		tempy[0]=dynamic_cast<gameObject*>(templ);
	}
	{
		glm::vec3 Varfe[]={
			glm::vec3(-1, 0,-1), //1  0
			glm::vec3(-1, 0, 1), //2  1
			glm::vec3( 1, 0, 1), //3  2
			glm::vec3( 1, 0,-1), //4  3

			glm::vec3(-1, 1,-1), //5  4
			glm::vec3(-1, 1, 1), //6  5
			glm::vec3( 1, 1, 1), //7  6
			glm::vec3( 1, 1,-1), //8  7
		};
		t_mesh Tri[]={
			t_mesh(		0,		0,		175,		2,		50,			226,		1,		50	,		175, true),//podea
			t_mesh(		0,		0,		175,		3,		0	,		226,		2,		50,			226, true),
			
			t_mesh(		4,		0,		227,		5,		125,		227,		6,		125,		295, true),//tavan
			t_mesh(		4,		0,		227,		6,		125,		295,		7,		0	,		295, true),
			
			t_mesh(		0,		0,		128,		7,		95 ,		175,		3,		95 ,		128, true),//latura1
			t_mesh(		0,		0,		128,		4,		0	,		175,		7,		95 ,		175, true),

			t_mesh(		1,		0,		128,		2,		95 ,		128,		6,		95 ,		175, true),//latura2
			t_mesh(		1,		0,		128,		6,		95 ,		175,		5,		0	,		175, true),

			t_mesh(		0,		0,		128,		1,		95 ,		128,		5,		95 ,		175, true),//latura3
			t_mesh(		0,		0,		128,		5,		95 ,		175,		4,		0	,		175, true),

			t_mesh(		3,		0,		128,		6,		95 ,		175,		2,		95 ,		128, true),//latura4
			t_mesh(		3,		0,		128,		7,		0	,		175,		6,		95 ,		175,true),
		};
		mesh meshy(Varfe,14,Tri,12,false);
		gameObject* templ=new gameObject(meshy.vf,meshy.nvf,meshy.cd,meshy.ncd,"Box Template");
		templ->_sx=templ->_sy=templ->_sz=500;
		templ->update();
		tempy[1]=templ;
	}
	{
		compositeGameObject *cgo = new compositeGameObject();
		cgo->nume="scena";
		cgo->real_name="scena";
		gameObject* Box = dynamic_cast<gameObject*>(tempy[1]->deepCopy());
		Box->nume="bounding_box";
		Box->real_name="bounding_box";
		cgo->children.push_back(Box);
	   
		char buff[999];
		for(int i=0;i<12;i++){
			slotMachine* Pacanea=dynamic_cast<slotMachine*>(tempy[0]->deepCopy());
			Pacanea->_tx=-350+10+i*(780.0f/12);
			Pacanea->_ty=50;
			Pacanea->_tz=-475;
			Pacanea->_fi=PI/2;
			sprintf(buff,"Pacanea_1_%d",i);
			Pacanea->nume=string(buff);
			Pacanea->real_name=string(buff);
			Pacanea->update();
			
			Pacanea->colliders.Add(treabaToCutie(Pacanea,mesagerie));
			cgo->children.push_back(dynamic_cast<gameObject*>(Pacanea));

		}
		for(int i=0;i<12;i++){
			slotMachine* Pacanea=dynamic_cast<slotMachine*>(tempy[0]->deepCopy());
			Pacanea->_tx=-475;
			Pacanea->_ty=50;
			Pacanea->_tz=-350+10+i*(780.0f/12);
			Pacanea->_fi=0;
			sprintf(buff,"Pacanea_2_%d",i);
			Pacanea->nume=string(buff);
			Pacanea->real_name=string(buff);
			Pacanea->update();
			
			Pacanea->colliders.Add(treabaToCutie(Pacanea,mesagerie));
			cgo->children.push_back(dynamic_cast<gameObject*>(Pacanea));
		}
		for(int i=0;i<12;i++){
			slotMachine* Pacanea=dynamic_cast<slotMachine*>(tempy[0]->deepCopy());
			Pacanea->_tx=-350+10+i*(780.0f/12);
			Pacanea->_ty=50;
			Pacanea->_tz=475;
			Pacanea->_fi=-PI/2;
			sprintf(buff,"Pacanea_3_%d",i);
			Pacanea->nume=string(buff);
			Pacanea->real_name=string(buff);
			Pacanea->update();
			
			Pacanea->colliders.Add(treabaToCutie(Pacanea,mesagerie));
			cgo->children.push_back(dynamic_cast<gameObject*>(Pacanea));
		}
		for(int i=0;i<12;i++){
			slotMachine* Pacanea=dynamic_cast<slotMachine*>(tempy[0]->deepCopy());
			Pacanea->_tx=475;
			Pacanea->_ty=50;
			Pacanea->_tz=-350+10+i*(780.0f/12);
			Pacanea->_fi=PI;
			sprintf(buff,"Pacanea_4_%d",i);
			Pacanea->nume=string(buff);
			Pacanea->real_name=string(buff);
			Pacanea->update();

			Pacanea->colliders.Add(treabaToCutie(Pacanea,mesagerie));
			cgo->children.push_back(dynamic_cast<gameObject*>(Pacanea));
		}
		cgo->update();
		tempy[2]=cgo;
	}
	{
		glm::vec3 Varfe[]={
			glm::vec3(-1, -1, 0), //1  0
			glm::vec3(-1,  1, 0), //2  1
			glm::vec3( 1,  1, 0), //3  2
			glm::vec3( 1, -1, 0), //4  3
		};
		t_mesh Tri[]={
			t_mesh(		0,		5,		6,			2,		7,		4,			1,		5,		4, true),//podea
			t_mesh(		0,		5,		6,			3,		7,		6,			2,		7,		4, true),
		};
		mesh meshy(Varfe,4,Tri,2,true);
		gameObject* templ=new gameObject(meshy.vf,meshy.nvf,meshy.cd,meshy.ncd,"Patrat Template");
		templ->_sx=templ->_sy=templ->_sz=100;
		templ->update();
		tempy[3]=templ;
	}

	{
		#define DISCR_S_F_N 17
		#define DISCR_S_T_N 32
		#define R 100
		glm::vec3 Varfe[DISCR_S_F_N * DISCR_S_T_N];
		t_mesh Tri[2*DISCR_S_F_N * DISCR_S_T_N];
		for(int i=0;i<DISCR_S_F_N;i++)
			for(int j=0;j<DISCR_S_T_N;j++){
				double fi=i*2*PI/DISCR_S_F_N;
				double theta=j*PI/DISCR_S_T_N;
				double x=R*sin(fi)*sin(theta);
				double y=R*sin(fi)*cos(theta);
				double z=R*cos(fi);
				Varfe[i*DISCR_S_F_N+j]=glm::vec3(x,y,z);
			}
		for(int i=0;i<DISCR_S_F_N;i++){
			for(int j=0;j<DISCR_S_T_N;j++){
				Tri[2*(i*DISCR_S_T_N+j) + 0]=t_mesh(
					(i*DISCR_S_T_N+j)				   , 0, 1,
					(i*DISCR_S_T_N+((j+1)%DISCR_S_T_N)), 0, 2, 
					(((i+1)%DISCR_S_F_N)*DISCR_S_T_N+j), 1, 1);
				Tri[2*(i*DISCR_S_T_N+j) + 1]=t_mesh(
					(((i+1)%DISCR_S_F_N)*DISCR_S_T_N+((j+1)%DISCR_S_T_N)), 1, 2,
					(i*DISCR_S_T_N+((j+1)%DISCR_S_T_N)), 0, 2, 
					(((i+1)%DISCR_S_F_N)*DISCR_S_T_N+j), 1, 1);
			}
		}
		mesh meshy(Varfe,DISCR_S_F_N * DISCR_S_T_N,Tri,2*DISCR_S_F_N * DISCR_S_T_N);
		gameObject* sfera=new gameObject(meshy.vf,meshy.nvf,meshy.cd,meshy.ncd,"Template bAnanas");
		tempy[5]=sfera;
		#undef DISCR_S_F_N
		#undef DISCR_S_T_N

	}
	
	{
		#define DISCR_S_F_N 17
		#define DISCR_S_T_N 32
		#define R 100
		glm::vec3 Varfe[DISCR_S_F_N * DISCR_S_T_N];
		t_mesh Tri[2*DISCR_S_F_N * DISCR_S_T_N];
		for(int i=0;i<DISCR_S_F_N;i++)
			for(int j=0;j<DISCR_S_T_N;j++){
				double fi=i*PI/DISCR_S_F_N;
				double theta=j*2*PI/DISCR_S_T_N;
				double x=R*sin(fi)*sin(theta);
				double y=R*sin(fi)*cos(theta);
				double z=R*cos(fi);
				Varfe[i*DISCR_S_T_N+j]=glm::vec3(x,y,z);
			}
		for(int i=0;i<DISCR_S_F_N;i++){
			for(int j=0;j<DISCR_S_T_N;j++){
				Tri[2*(i*DISCR_S_T_N+j) + 0]=t_mesh(
					(i*DISCR_S_T_N+j)				   , 224+20 + (rand()%30)*0.001f, 192+20,
					(i*DISCR_S_T_N+((j+1)%DISCR_S_T_N)), 224+20 + (rand()%30)*0.001f, 192+20, 
					(((i+1)%DISCR_S_F_N)*DISCR_S_T_N+j), 224+20 + (rand()%30)*0.001f, 192+20);
				Tri[2*(i*DISCR_S_T_N+j) + 1]=t_mesh(
					(((i+1)%DISCR_S_F_N)*DISCR_S_T_N+((j+1)%DISCR_S_T_N)), 224 + (rand()%30)*0.001f+20, 192+20,
					(i*DISCR_S_T_N+((j+1)%DISCR_S_T_N)), 224 + (rand()%30)*0.001f+20, 192+20, 
					(((i+1)%DISCR_S_F_N)*DISCR_S_T_N+j), 224 + (rand()%30)*0.001f+20, 192+20,true);
			}
		}
		mesh meshy(Varfe,DISCR_S_F_N * DISCR_S_T_N,Tri,2*DISCR_S_F_N * DISCR_S_T_N,false);
		gameObject* sfera=new gameObject(meshy.vf,meshy.nvf,meshy.cd,meshy.ncd,"Template Sfera");
		tempy[4]=sfera;
		#undef DISCR_S_F_N
		#undef DISCR_S_T_N

	}
	compositeGameObject* room1 = dynamic_cast<compositeGameObject*>(tempy[2]->deepCopy());
	room1->nume="Camera_1";
	room1->real_name="Camera_1";
	room1->proprii.Add(treabaToCutie(room1,mesagerie));
	gameManager.children.push_back(room1);
	printf("%d",gameManager.children.size());

	compositeGameObject* room2 = dynamic_cast<compositeGameObject*>(tempy[2]->deepCopy());
	room2->nume="Camera_2";
	room2->real_name="Camera_2";
	room2->proprii.Add(treabaToCutie(room2,mesagerie));
	room2->update();
	room2->_tx=1001;
	room2->update();

	gameObject* sfera=tempy[4]->deepCopy();
	sfera->nume="Sfera";
	sfera->real_name="Sfera";
	sfera->_ty=400;
	sfera->_sx=0.5;
	sfera->_sz=0.5;
	sfera->_sy=0.5;
	sfera->_fi+=PI/2;
	room2->children.push_back(sfera);

	{
		compositeGameObject* casca=new compositeGameObject();
		casca->nume="bAnanas";
		casca->real_name="bAnanas";
		gameObject* sfera2=tempy[5]->deepCopy();
		sfera2->nume="bAnanas";
		sfera2->real_name="bAnanas";
		sfera2->_ty=400;
		sfera2->_sx=0.6;
		sfera2->_sz=0.6;
		sfera2->_sy=0.6;
		sfera2->_fi+=PI/2;

		gameObject* ochi1=tempy[4]->deepCopy();
		ochi1->nume="Ochi_1";
		ochi1->real_name="Ochi_1";
		ochi1->_ty=385;
		ochi1->_tz=50*cos(PI/6);
		ochi1->_tx=50*sin(PI/6);
		ochi1->_sx=0.1;
		ochi1->_sy=0.1;
		ochi1->_sz=0.1;
		for(int i=0;i<ochi1->orig_vf.size()/9;i++){
			ochi1->orig_vf[9*i + 7] = 453 + (rand()%30)*0.001f;//160+64*2+32;
			ochi1->orig_vf[9*i + 8] = 268;
		}
		
		gameObject* ochi2=tempy[4]->deepCopy();
		ochi2->nume="Ochi_2";
		ochi2->real_name="Ochi_2";
		ochi2->_ty=385;
		ochi2->_tz=50*cos(-PI/6);
		ochi2->_tx=50*sin(-PI/6);
		ochi2->_sx=0.1;
		ochi2->_sy=0.1;
		ochi2->_sz=0.1;
		for(int i=0;i<ochi2->orig_vf.size()/9;i++){
			ochi2->orig_vf[9*i + 7] = 8 + (rand()%30)*0.001f;
			ochi2->orig_vf[9*i + 8] = 8+32;
		}
		
		gameObject* gura=tempy[4]->deepCopy();
		gura->nume="Gura";
		gura->real_name="Gura";
		gura->_ty=360;
		gura->_tz=20;
		gura->_sx=0.3;
		gura->_sy=0.05;
		gura->_sz=0.1;
		gura->_fi=PI;
		for(int i=0;i<gura->orig_vf.size()/9;i++){
			gura->orig_vf[9*i + 7] = 160 + (rand()%30)*0.001f;
			gura->orig_vf[9*i + 8] = 128;
		}
		
		casca->children.push_back(ochi1);
		casca->children.push_back(ochi2);
		casca->children.push_back(gura);
		casca->children.push_back(sfera2);
		room2->children.push_back(casca);
	}

	gameManager.children.push_back(room2);
	gameManager.nume="_root";
	gameManager.real_name="_root";
	gameManager.update();
	permise.C.push_back(rect3(-450.0,450.0,0.0,500.0,-450.0,450.0,0,0));
	permise.C.push_back(rect3(-450.0+1000,450.0+1000,0.0,500.0,-450.0,450.0,0,0));
	image=SOIL_load_image("cutie ASTA.png", &imgwidth, &imgheight, 0, SOIL_LOAD_RGB); 
	//cout<<"img: "<<imgwidth<<" "<<imgheight<<"\n";
	//system("PAUSE");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowPosition (100,100); 
	glutInitWindowSize(1200,900); 
	glutCreateWindow("Paciniele"); 
	glewInit(); 
	Initialize( );



	glutIdleFunc(RenderFunction);
	glutDisplayFunc(RenderFunction);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutCloseFunc(Cleanup);
	glutMainLoop(); 
}
