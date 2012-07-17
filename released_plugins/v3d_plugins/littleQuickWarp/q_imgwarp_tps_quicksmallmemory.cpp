//imgwarp_tps_quicksmallmemory.cpp
//
// by Lei Qu
//2012-07-08

#include "q_interpolate.h"

//TPS_linear_blockbyblock image warping
template <class T>
bool imgwarp_smallmemory(const T *p_img_sub,const V3DLONG *sz_img_sub,
		const QList<ImageMarker> &ql_marker_tar,const QList<ImageMarker> &ql_marker_sub,
		V3DLONG szBlock_x, V3DLONG szBlock_y, V3DLONG szBlock_z,
		T *&p_img_warp)
{
	//check parameters
	if(p_img_sub==0 || sz_img_sub==0)
	{
		printf("ERROR: p_img_sub or sz_img_sub is invalid.\n");
		return false;
	}
	if(ql_marker_tar.size()==0 || ql_marker_sub.size()==0 || ql_marker_tar.size()!=ql_marker_sub.size())
	{
		printf("ERROR: target or subject control points is invalid!\n");
		return false;
	}
	if(szBlock_x<=0 || szBlock_y<=0 || szBlock_z<=0)
	{
		printf("ERROR: block size is invalid!\n");
		return false;
	}
	if(szBlock_x>=sz_img_sub[0] || szBlock_y>=sz_img_sub[1] || szBlock_z>=sz_img_sub[2])
	{
		printf("ERROR: block size should smaller than the image size!\n");
		return false;
	}
	if(p_img_warp)
	{
		printf("WARNNING: output image pointer is not null, original memeroy it point to will be released!\n");
		if(p_img_warp) 			{delete []p_img_warp;		p_img_warp=0;}
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	printf(">>>>compute the subsampled displace field \n");
    vector<Coord3D_JBA> matchTargetPos, matchSubjectPos;
	for (V3DLONG i=0;i<ql_marker_tar.size();i++)
	{
		Coord3D_JBA tmpc;
		tmpc.x=ql_marker_tar.at(i).x;	tmpc.y=ql_marker_tar.at(i).y;	tmpc.z=ql_marker_tar.at(i).z;
		matchTargetPos.push_back(tmpc);
		tmpc.x=ql_marker_sub.at(i).x;	tmpc.y=ql_marker_sub.at(i).y;	tmpc.z=ql_marker_sub.at(i).z;
		matchSubjectPos.push_back(tmpc);
	}
	Vol3DSimple<DisplaceFieldF3D> *pSubDF	=compute_df_tps_subsampled_volume(matchTargetPos,matchSubjectPos,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],szBlock_x,szBlock_y,szBlock_z);
	if(!pSubDF)
	{
		printf("Fail to produce the subsampled DF.\n");
		return false;
	}
	DisplaceFieldF3D 	        ***pppSubDF	=pSubDF->getData3dHandle();
	printf("subsampled DF size: [%ld,%ld,%ld]\n",pSubDF->sz0(),pSubDF->sz1(),pSubDF->sz2());

	//------------------------------------------------------------------------
	//allocate memory
	printf(">>>>interpolate the subsampled displace field and warp block by block\n");
	p_img_warp=new T[sz_img_sub[0]*sz_img_sub[1]*sz_img_sub[2]*sz_img_sub[3]]();
	if(!p_img_warp)
	{
		printf("ERROR: Fail to allocate memory for p_img_warp.\n");
		if(pSubDF)				{delete pSubDF;					pSubDF=0;}
		return false;
	}
	T ****p_img_warp_4d=0,****p_img_sub_4d=0;
	if(!new4dpointer(p_img_warp_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3],p_img_warp) ||
	   !new4dpointer(p_img_sub_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3],p_img_sub))
	{
		printf("ERROR: Fail to allocate memory for the 4d pointer of image.\n");
		if(p_img_warp_4d) 		{delete4dpointer(p_img_warp_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3]);}
		if(p_img_sub_4d) 		{delete4dpointer(p_img_sub_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3]);}
		if(p_img_warp) 			{delete []p_img_warp;			p_img_warp=0;}
		if(pSubDF)				{delete pSubDF;					pSubDF=0;}
		return false;
	}
	Vol3DSimple<MYFLOAT_JBA> 		*pSubDFBlock1c	=new Vol3DSimple<MYFLOAT_JBA> (2, 2, 2);
	if(!pSubDFBlock1c)
	{
		printf("ERROR: Fail to allocate memory for pSubDFBlock1c.\n");
		if(p_img_warp_4d) 		{delete4dpointer(p_img_warp_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3]);}
		if(p_img_sub_4d) 		{delete4dpointer(p_img_sub_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3]);}
		if(p_img_warp) 			{delete []p_img_warp;			p_img_warp=0;}
		if(pSubDF)				{delete pSubDF;					pSubDF=0;}
		return false;
	}
    MYFLOAT_JBA 		     	***pppSubDFBlock1c	=pSubDFBlock1c->getData3dHandle();
	Vol3DSimple<DisplaceFieldF3D> 	*pDFBlock		=new Vol3DSimple<DisplaceFieldF3D> (szBlock_x,szBlock_y,szBlock_z);
	if(!pSubDFBlock1c)
	{
		printf("ERROR: Fail to allocate memory for pDFBlock.\n");
		if(p_img_warp_4d) 		{delete4dpointer(p_img_warp_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3]);}
		if(p_img_sub_4d) 		{delete4dpointer(p_img_sub_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3]);}
		if(p_img_warp) 			{delete []p_img_warp;			p_img_warp=0;}
		if(pSubDFBlock1c)		{delete pSubDFBlock1c;			pSubDFBlock1c=0;}
		if(pSubDF)				{delete pSubDF;					pSubDF=0;}
		return false;
	}
	DisplaceFieldF3D 	        ***pppDFBlock		=pDFBlock->getData3dHandle();
	Vol3DSimple <MYFLOAT_JBA> 		*pDFBlock1C		=0;
	MYFLOAT_JBA			    	***pppDFBlock1C		=0;

	//warp image block by block
	for(V3DLONG substart_z=0;substart_z<pSubDF->sz2()-1;substart_z++)
		for(V3DLONG substart_y=0;substart_y<pSubDF->sz1()-1;substart_y++)
			for(V3DLONG substart_x=0;substart_x<pSubDF->sz0()-1;substart_x++)
			{
				//linear interpolate the SubDFBlock to DFBlock
				V3DLONG i,j,k,is,js,ks;
				//x
				for(k=0;k<2;k++)
					for(j=0;j<2;j++)
						for(i=0;i<2;i++)
						{
							ks=k+substart_z;
							js=j+substart_y;
							is=i+substart_x;
							pppSubDFBlock1c[k][j][i]=pppSubDF[ks][js][is].sx;
						}
			 	pDFBlock1C=linearinterp_regularmesh_3d(szBlock_x,szBlock_y,szBlock_z,pSubDFBlock1c);
				pppDFBlock1C=pDFBlock1C->getData3dHandle();
				for(k=0;k<szBlock_z;k++) for(j=0;j<szBlock_y;j++) for(i=0;i<szBlock_x;i++) pppDFBlock[k][j][i].sx=pppDFBlock1C[k][j][i];
				if(pDFBlock1C) {delete pDFBlock1C; pDFBlock1C=0;}
				//y
				for(k=0;k<2;k++)
					for(j=0;j<2;j++)
						for(i=0;i<2;i++)
						{
							ks=k+substart_z;
							js=j+substart_y;
							is=i+substart_x;
							pppSubDFBlock1c[k][j][i]=pppSubDF[ks][js][is].sy;
						}
			 	pDFBlock1C=linearinterp_regularmesh_3d(szBlock_x,szBlock_y,szBlock_z,pSubDFBlock1c);
				pppDFBlock1C=pDFBlock1C->getData3dHandle();
				for(k=0;k<szBlock_z;k++) for(j=0;j<szBlock_y;j++) for(i=0;i<szBlock_x;i++) pppDFBlock[k][j][i].sy=pppDFBlock1C[k][j][i];
				if(pDFBlock1C) {delete pDFBlock1C; pDFBlock1C=0;}
				//z
				for(k=0;k<2;k++)
					for(j=0;j<2;j++)
						for(i=0;i<2;i++)
						{
							ks=k+substart_z;
							js=j+substart_y;
							is=i+substart_x;
							pppSubDFBlock1c[k][j][i]=pppSubDF[ks][js][is].sz;
						}
			 	pDFBlock1C=linearinterp_regularmesh_3d(szBlock_x,szBlock_y,szBlock_z,pSubDFBlock1c);
				pppDFBlock1C=pDFBlock1C->getData3dHandle();
				for(k=0;k<szBlock_z;k++) for(j=0;j<szBlock_y;j++) for(i=0;i<szBlock_x;i++) pppDFBlock[k][j][i].sz=pppDFBlock1C[k][j][i];
				if(pDFBlock1C) {delete pDFBlock1C; pDFBlock1C=0;}

				//warp image block using DFBlock
				V3DLONG start_x,start_y,start_z;
				start_x=substart_x*szBlock_x;
				start_y=substart_y*szBlock_y;
				start_z=substart_z*szBlock_z;
				for(V3DLONG z=0;z<szBlock_z;z++)
					for(V3DLONG y=0;y<szBlock_y;y++)
						for(V3DLONG x=0;x<szBlock_x;x++)
						{
							V3DLONG pos_warp[3];
							pos_warp[0]=start_x+x;
							pos_warp[1]=start_y+y;
							pos_warp[2]=start_z+z;
							if(pos_warp[0]>=sz_img_sub[0] || pos_warp[1]>=sz_img_sub[1] || pos_warp[2]>=sz_img_sub[2])
								continue;

							double pos_sub[3];
							pos_sub[0]=pos_warp[0]+pppDFBlock[z][y][x].sx;
							pos_sub[1]=pos_warp[1]+pppDFBlock[z][y][x].sy;
							pos_sub[2]=pos_warp[2]+pppDFBlock[z][y][x].sz;
							if(pos_sub[0]<0 || pos_sub[0]>sz_img_sub[0]-1 ||
							   pos_sub[1]<0 || pos_sub[1]>sz_img_sub[1]-1 ||
							   pos_sub[2]<0 || pos_sub[2]>sz_img_sub[2]-1)
							{
								for(V3DLONG c=0;c<sz_img_sub[3];c++)
									p_img_warp_4d[c][pos_warp[2]][pos_warp[1]][pos_warp[0]]=0;
								continue;
							}

							//linear interpolate
							//find 8 neighor pixels boundary
							V3DLONG x_s,x_b,y_s,y_b,z_s,z_b;
							x_s=floor(pos_sub[0]);		x_b=ceil(pos_sub[0]);
							y_s=floor(pos_sub[1]);		y_b=ceil(pos_sub[1]);
							z_s=floor(pos_sub[2]);		z_b=ceil(pos_sub[2]);

							//compute weight for left and right, top and bottom -- 4 neighbor pixel's weight in a slice
							double l_w,r_w,t_w,b_w;
							l_w=1.0-(pos_sub[0]-x_s);	r_w=1.0-l_w;
							t_w=1.0-(pos_sub[1]-y_s);	b_w=1.0-t_w;
							//compute weight for higer slice and lower slice
							double u_w,d_w;
							u_w=1.0-(pos_sub[2]-z_s);	d_w=1.0-u_w;

							//linear interpolate each channel
							for(V3DLONG c=0;c<sz_img_sub[3];c++)
							{
								//linear interpolate in higher slice [t_w*(l_w*lt+r_w*rt)+b_w*(l_w*lb+r_w*rb)]
								double higher_slice;
								higher_slice=t_w*(l_w*p_img_sub_4d[c][z_s][y_s][x_s]+r_w*p_img_sub_4d[c][z_s][y_s][x_b])+
											 b_w*(l_w*p_img_sub_4d[c][z_s][y_b][x_s]+r_w*p_img_sub_4d[c][z_s][y_b][x_b]);
								//linear interpolate in lower slice [t_w*(l_w*lt+r_w*rt)+b_w*(l_w*lb+r_w*rb)]
								double lower_slice;
								lower_slice =t_w*(l_w*p_img_sub_4d[c][z_b][y_s][x_s]+r_w*p_img_sub_4d[c][z_b][y_s][x_b])+
											 b_w*(l_w*p_img_sub_4d[c][z_b][y_b][x_s]+r_w*p_img_sub_4d[c][z_b][y_b][x_b]);
								//linear interpolate the current position [u_w*higher_slice+d_w*lower_slice]
								T intval=(T)(u_w*higher_slice+d_w*lower_slice+0.5);
								p_img_warp_4d[c][pos_warp[2]][pos_warp[1]][pos_warp[0]]=intval;
							}
						}
			}

	//free memory
	if(p_img_warp_4d) 		{delete4dpointer(p_img_warp_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3]);}
	if(p_img_sub_4d) 		{delete4dpointer(p_img_sub_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3]);}
	if(pSubDFBlock1c)		{delete pSubDFBlock1c;		pSubDFBlock1c=0;}
	if(pDFBlock)			{delete pDFBlock;			pDFBlock=0;}
	if(pSubDF)				{delete pSubDF;				pSubDF=0;}

	return true;
}

//TPS_linear_blockbyblock image warping
//padding and recentering image before warping, in order to generate same result as JBA (just for comparision)
//note: need to release useless memory as early as possible to save momory
template <class T>
bool imgwarp_smallmemory_padding(const T *p_img_sub,const V3DLONG *sz_img_sub,
		const QList<ImageMarker> &ql_marker_tar,const QList<ImageMarker> &ql_marker_sub,
		V3DLONG szBlock_x, V3DLONG szBlock_y, V3DLONG szBlock_z,
		T *&p_img_warp)
{
	//check parameters
	if(p_img_sub==0 || sz_img_sub==0)
	{
		printf("ERROR: p_img_sub or sz_img_sub is invalid.\n");
		return false;
	}
	if(ql_marker_tar.size()==0 || ql_marker_sub.size()==0 || ql_marker_tar.size()!=ql_marker_sub.size())
	{
		printf("ERROR: target or subject control points is invalid!\n");
		return false;
	}
	if(szBlock_x<=0 || szBlock_y<=0 || szBlock_z<=0)
	{
		printf("ERROR: block size is invalid!\n");
		return false;
	}
	if(szBlock_x>=sz_img_sub[0] || szBlock_y>=sz_img_sub[1] || szBlock_z>=sz_img_sub[2])
	{
		printf("ERROR: block size should smaller than the image size!\n");
		return false;
	}
	if(p_img_warp)
	{
		printf("WARNNING: output image pointer is not null, original memeroy it point to will be released!\n");
		if(p_img_warp) 			{delete []p_img_warp;		p_img_warp=0;}
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	printf(">>>>compute offset and size for padding and recentering image \n");
	V3DLONG sz_img_sub_padding[4],offset_padding[3];
	sz_img_sub_padding[3]=sz_img_sub[3];
	for(int i=0;i<3;i++)
	{
		sz_img_sub_padding[i]=V3DLONG(ceil(double(sz_img_sub[i])/32))*32;
		offset_padding[i]=floor((sz_img_sub_padding[i]-sz_img_sub[i])/2);//this sentense has problem! anyway we leave it along since we want to generate same result as jba
	}
	printf("sz_img_padding=[%ld,%ld,%ld],offset_padding=[%ld,%ld,%ld]\n",sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],offset_padding[0],offset_padding[1],offset_padding[2]);
	//allocate memory
	T *p_img_sub_padding=0;
	p_img_sub_padding=new T[sz_img_sub_padding[0]*sz_img_sub_padding[1]*sz_img_sub_padding[2]*sz_img_sub_padding[3]]();
	if(!p_img_sub_padding)
	{
		printf("ERROR: Fail to allocate memory for p_img_sub_padding.\n");
		return false;
	}
	T ****p_img_sub_4d=0,****p_img_sub_padding_4d=0;
	if(!new4dpointer(p_img_sub_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3],p_img_sub) ||
	   !new4dpointer(p_img_sub_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3],p_img_sub_padding))
	{
		printf("ERROR: Fail to allocate memory for the 4d pointer of image.\n");
		if(p_img_sub_4d) 			{delete4dpointer(p_img_sub_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3]);}
		if(p_img_sub_padding_4d)	{delete4dpointer(p_img_sub_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3]);}
		if(p_img_sub_padding) 		{delete []p_img_sub_padding;		p_img_sub_padding=0;}
		return false;
	}
	//padding and recentering
	for(V3DLONG c=0;c<sz_img_sub[3];c++)
		for(V3DLONG z=0;z<sz_img_sub[2];z++)
			for(V3DLONG y=0;y<sz_img_sub[1];y++)
				for(V3DLONG x=0;x<sz_img_sub[0];x++)
					p_img_sub_padding_4d[c][z+offset_padding[2]][y+offset_padding[1]][x+offset_padding[0]]=p_img_sub_4d[c][z][y][x];
	if(p_img_sub_4d) 				{delete4dpointer(p_img_sub_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3]);}

	//------------------------------------------------------------------------
	printf(">>>>compute the subsampled displace field \n");
    vector<Coord3D_JBA> matchTargetPos, matchSubjectPos;
	for (V3DLONG i=0;i<ql_marker_tar.size();i++)
	{
		Coord3D_JBA tmpc;
		tmpc.x=ql_marker_tar.at(i).x+offset_padding[0];	tmpc.y=ql_marker_tar.at(i).y+offset_padding[1];	tmpc.z=ql_marker_tar.at(i).z+offset_padding[2];
		matchTargetPos.push_back(tmpc);
		tmpc.x=ql_marker_sub.at(i).x+offset_padding[0];	tmpc.y=ql_marker_sub.at(i).y+offset_padding[1];	tmpc.z=ql_marker_sub.at(i).z+offset_padding[2];
		matchSubjectPos.push_back(tmpc);
	}
	Vol3DSimple<DisplaceFieldF3D> *pSubDF	=compute_df_tps_subsampled_volume(matchTargetPos,matchSubjectPos,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],szBlock_x,szBlock_y,szBlock_z);
	if(!pSubDF)
	{
		printf("Fail to produce the subsampled DF.\n");
		if(p_img_sub_padding_4d)	{delete4dpointer(p_img_sub_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3]);}
		if(p_img_sub_padding) 		{delete []p_img_sub_padding;		p_img_sub_padding=0;}
		return false;
	}
	DisplaceFieldF3D 	        ***pppSubDF	=pSubDF->getData3dHandle();
	printf("subsampled DF size: [%ld,%ld,%ld]\n",pSubDF->sz0(),pSubDF->sz1(),pSubDF->sz2());

	//------------------------------------------------------------------------
	//allocate memory for block by block warping
	printf(">>>>interpolate the subsampled displace field and warp block by block\n");
	T *p_img_warp_padding=0;
	p_img_warp_padding=new T[sz_img_sub_padding[0]*sz_img_sub_padding[1]*sz_img_sub_padding[2]*sz_img_sub_padding[3]]();
	if(!p_img_warp_padding)
	{
		printf("ERROR: Fail to allocate memory for p_img_warp.\n");
		if(p_img_sub_padding_4d){delete4dpointer(p_img_sub_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3]);}
		if(p_img_sub_padding) 		{delete []p_img_sub_padding;		p_img_sub_padding=0;}
		if(pSubDF)					{delete pSubDF;						pSubDF=0;}
		return false;
	}
	T ****p_img_warp_padding_4d=0;
	if(!new4dpointer(p_img_warp_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3],p_img_warp_padding))
	{
		printf("ERROR: Fail to allocate memory for the 4d pointer of image.\n");
		if(p_img_warp_padding_4d) 	{delete4dpointer(p_img_warp_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3]);}
		if(p_img_sub_padding_4d)	{delete4dpointer(p_img_sub_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3]);}
		if(p_img_warp_padding) 		{delete []p_img_warp_padding;		p_img_warp_padding=0;}
		if(p_img_sub_padding) 		{delete []p_img_sub_padding;		p_img_sub_padding=0;}
		if(pSubDF)					{delete pSubDF;						pSubDF=0;}
		return false;
	}
	Vol3DSimple<MYFLOAT_JBA> 		*pSubDFBlock1c	=new Vol3DSimple<MYFLOAT_JBA> (2, 2, 2);
	if(!pSubDFBlock1c)
	{
		printf("ERROR: Fail to allocate memory for pSubDFBlock1c.\n");
		if(p_img_warp_padding_4d) 	{delete4dpointer(p_img_warp_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3]);}
		if(p_img_sub_padding_4d)	{delete4dpointer(p_img_sub_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3]);}
		if(p_img_warp_padding) 		{delete []p_img_warp_padding;		p_img_warp_padding=0;}
		if(p_img_sub_padding) 		{delete []p_img_sub_padding;		p_img_sub_padding=0;}
		if(pSubDF)					{delete pSubDF;						pSubDF=0;}
		return false;
	}
    MYFLOAT_JBA 		     	***pppSubDFBlock1c	=pSubDFBlock1c->getData3dHandle();
	Vol3DSimple<DisplaceFieldF3D> 	*pDFBlock		=new Vol3DSimple<DisplaceFieldF3D> (szBlock_x,szBlock_y,szBlock_z);
	if(!pSubDFBlock1c)
	{
		printf("ERROR: Fail to allocate memory for pDFBlock.\n");
		if(p_img_warp_padding_4d) 	{delete4dpointer(p_img_warp_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3]);}
		if(p_img_sub_padding_4d)	{delete4dpointer(p_img_sub_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3]);}
		if(p_img_warp_padding) 		{delete []p_img_warp_padding;		p_img_warp_padding=0;}
		if(p_img_sub_padding) 		{delete []p_img_sub_padding;		p_img_sub_padding=0;}
		if(pSubDFBlock1c)			{delete pSubDFBlock1c;				pSubDFBlock1c=0;}
		if(pSubDF)					{delete pSubDF;						pSubDF=0;}
		return false;
	}
	DisplaceFieldF3D 	        ***pppDFBlock		=pDFBlock->getData3dHandle();
	Vol3DSimple <MYFLOAT_JBA> 		*pDFBlock1C		=0;
	MYFLOAT_JBA			    	***pppDFBlock1C		=0;

	//------------------------------------------------------------------------
	//warp image block by block
	for(V3DLONG substart_z=0;substart_z<pSubDF->sz2()-1;substart_z++)
		for(V3DLONG substart_y=0;substart_y<pSubDF->sz1()-1;substart_y++)
			for(V3DLONG substart_x=0;substart_x<pSubDF->sz0()-1;substart_x++)
			{
				//linear interpolate the SubDFBlock to DFBlock
				V3DLONG i,j,k,is,js,ks;
				//x
				for(k=0;k<2;k++)
					for(j=0;j<2;j++)
						for(i=0;i<2;i++)
						{
							ks=k+substart_z;
							js=j+substart_y;
							is=i+substart_x;
							pppSubDFBlock1c[k][j][i]=pppSubDF[ks][js][is].sx;
						}
			 	pDFBlock1C=linearinterp_regularmesh_3d(szBlock_x,szBlock_y,szBlock_z,pSubDFBlock1c);
				pppDFBlock1C=pDFBlock1C->getData3dHandle();
				for(k=0;k<szBlock_z;k++) for(j=0;j<szBlock_y;j++) for(i=0;i<szBlock_x;i++) pppDFBlock[k][j][i].sx=pppDFBlock1C[k][j][i];
				if(pDFBlock1C) {delete pDFBlock1C; pDFBlock1C=0;}
				//y
				for(k=0;k<2;k++)
					for(j=0;j<2;j++)
						for(i=0;i<2;i++)
						{
							ks=k+substart_z;
							js=j+substart_y;
							is=i+substart_x;
							pppSubDFBlock1c[k][j][i]=pppSubDF[ks][js][is].sy;
						}
			 	pDFBlock1C=linearinterp_regularmesh_3d(szBlock_x,szBlock_y,szBlock_z,pSubDFBlock1c);
				pppDFBlock1C=pDFBlock1C->getData3dHandle();
				for(k=0;k<szBlock_z;k++) for(j=0;j<szBlock_y;j++) for(i=0;i<szBlock_x;i++) pppDFBlock[k][j][i].sy=pppDFBlock1C[k][j][i];
				if(pDFBlock1C) {delete pDFBlock1C; pDFBlock1C=0;}
				//z
				for(k=0;k<2;k++)
					for(j=0;j<2;j++)
						for(i=0;i<2;i++)
						{
							ks=k+substart_z;
							js=j+substart_y;
							is=i+substart_x;
							pppSubDFBlock1c[k][j][i]=pppSubDF[ks][js][is].sz;
						}
			 	pDFBlock1C=linearinterp_regularmesh_3d(szBlock_x,szBlock_y,szBlock_z,pSubDFBlock1c);
				pppDFBlock1C=pDFBlock1C->getData3dHandle();
				for(k=0;k<szBlock_z;k++) for(j=0;j<szBlock_y;j++) for(i=0;i<szBlock_x;i++) pppDFBlock[k][j][i].sz=pppDFBlock1C[k][j][i];
				if(pDFBlock1C) {delete pDFBlock1C; pDFBlock1C=0;}

				//warp image block using DFBlock
				V3DLONG start_x,start_y,start_z;
				start_x=substart_x*szBlock_x;
				start_y=substart_y*szBlock_y;
				start_z=substart_z*szBlock_z;
				for(V3DLONG z=0;z<szBlock_z;z++)
					for(V3DLONG y=0;y<szBlock_y;y++)
						for(V3DLONG x=0;x<szBlock_x;x++)
						{
							V3DLONG pos_warp[3];
							pos_warp[0]=start_x+x;
							pos_warp[1]=start_y+y;
							pos_warp[2]=start_z+z;
							if(pos_warp[0]>=sz_img_sub_padding[0] || pos_warp[1]>=sz_img_sub_padding[1] || pos_warp[2]>=sz_img_sub_padding[2])
								continue;

							double pos_sub[3];
							pos_sub[0]=pos_warp[0]+pppDFBlock[z][y][x].sx;
							pos_sub[1]=pos_warp[1]+pppDFBlock[z][y][x].sy;
							pos_sub[2]=pos_warp[2]+pppDFBlock[z][y][x].sz;
							if(pos_sub[0]<0 || pos_sub[0]>sz_img_sub_padding[0]-1 ||
							   pos_sub[1]<0 || pos_sub[1]>sz_img_sub_padding[1]-1 ||
							   pos_sub[2]<0 || pos_sub[2]>sz_img_sub_padding[2]-1)
							{
								for(V3DLONG c=0;c<sz_img_sub_padding[3];c++)
									p_img_warp_padding_4d[c][pos_warp[2]][pos_warp[1]][pos_warp[0]]=0;
								continue;
							}

							//linear interpolate
							//find 8 neighor pixels boundary
							V3DLONG x_s,x_b,y_s,y_b,z_s,z_b;
							x_s=floor(pos_sub[0]);		x_b=ceil(pos_sub[0]);
							y_s=floor(pos_sub[1]);		y_b=ceil(pos_sub[1]);
							z_s=floor(pos_sub[2]);		z_b=ceil(pos_sub[2]);

							//compute weight for left and right, top and bottom -- 4 neighbor pixel's weight in a slice
							double l_w,r_w,t_w,b_w;
							l_w=1.0-(pos_sub[0]-x_s);	r_w=1.0-l_w;
							t_w=1.0-(pos_sub[1]-y_s);	b_w=1.0-t_w;
							//compute weight for higer slice and lower slice
							double u_w,d_w;
							u_w=1.0-(pos_sub[2]-z_s);	d_w=1.0-u_w;

							//linear interpolate each channel
							for(V3DLONG c=0;c<sz_img_sub_padding[3];c++)
							{
								//linear interpolate in higher slice [t_w*(l_w*lt+r_w*rt)+b_w*(l_w*lb+r_w*rb)]
								double higher_slice;
								higher_slice=t_w*(l_w*p_img_sub_padding_4d[c][z_s][y_s][x_s]+r_w*p_img_sub_padding_4d[c][z_s][y_s][x_b])+
											 b_w*(l_w*p_img_sub_padding_4d[c][z_s][y_b][x_s]+r_w*p_img_sub_padding_4d[c][z_s][y_b][x_b]);
								//linear interpolate in lower slice [t_w*(l_w*lt+r_w*rt)+b_w*(l_w*lb+r_w*rb)]
								double lower_slice;
								lower_slice =t_w*(l_w*p_img_sub_padding_4d[c][z_b][y_s][x_s]+r_w*p_img_sub_padding_4d[c][z_b][y_s][x_b])+
											 b_w*(l_w*p_img_sub_padding_4d[c][z_b][y_b][x_s]+r_w*p_img_sub_padding_4d[c][z_b][y_b][x_b]);
								//linear interpolate the current position [u_w*higher_slice+d_w*lower_slice]
//								T intval=(T)(u_w*higher_slice+d_w*lower_slice+0.5);
								T intval=(T)(u_w*higher_slice+d_w*lower_slice);//in jba, we just truncate it, keep same here
								p_img_warp_padding_4d[c][pos_warp[2]][pos_warp[1]][pos_warp[0]]=intval;
							}

						}
			}
	if(p_img_sub_padding_4d){delete4dpointer(p_img_sub_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3]);}
	if(p_img_sub_padding) 	{delete []p_img_sub_padding;p_img_sub_padding=0;}
	if(pSubDFBlock1c)		{delete pSubDFBlock1c;		pSubDFBlock1c=0;}
	if(pDFBlock)			{delete pDFBlock;			pDFBlock=0;}
	if(pSubDF)				{delete pSubDF;				pSubDF=0;}

	//------------------------------------------------------------------------
	//unpadding
	p_img_warp=new T[sz_img_sub[0]*sz_img_sub[1]*sz_img_sub[2]*sz_img_sub[3]]();
	if(!p_img_warp)
	{
		printf("ERROR: Fail to allocate memory for p_img_warp.\n");
		if(p_img_warp_padding) 			{delete []p_img_warp_padding;p_img_warp_padding=0;}
		if(p_img_warp_padding_4d) 		{delete4dpointer(p_img_warp_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3]);}
		return false;
	}
	T ****p_img_warp_4d=0;
	if(!new4dpointer(p_img_warp_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3],p_img_warp))
	{
		printf("ERROR: Fail to allocate memory for the 4d pointer of image.\n");
		if(p_img_warp_padding_4d) 		{delete4dpointer(p_img_warp_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3]);}
		if(p_img_warp_padding) 			{delete []p_img_warp_padding;p_img_warp_padding=0;}
		if(p_img_warp) 					{delete []p_img_warp;p_img_warp=0;}
		return false;
	}
	for(V3DLONG c=0;c<sz_img_sub[3];c++)
		for(V3DLONG z=0;z<sz_img_sub[2];z++)
			for(V3DLONG y=0;y<sz_img_sub[1];y++)
				for(V3DLONG x=0;x<sz_img_sub[0];x++)
					p_img_warp_4d[c][z][y][x]=p_img_warp_padding_4d[c][z+offset_padding[2]][y+offset_padding[1]][x+offset_padding[0]];
	if(p_img_warp_4d) 				{delete4dpointer(p_img_warp_4d,sz_img_sub[0],sz_img_sub[1],sz_img_sub[2],sz_img_sub[3]);}
	if(p_img_warp_padding_4d) 		{delete4dpointer(p_img_warp_padding_4d,sz_img_sub_padding[0],sz_img_sub_padding[1],sz_img_sub_padding[2],sz_img_sub_padding[3]);}
	if(p_img_warp_padding) 			{delete []p_img_warp_padding;p_img_warp_padding=0;}

	return true;
}

//inline resize image by padding and unpadding
template <class T>
bool imgresize_padding(T *&p_img,V3DLONG *sz_img,
		V3DLONG sz_resize_x, V3DLONG sz_resize_y, V3DLONG sz_resize_z)
{
	//check parameters
	if(p_img==0 || sz_img==0)
	{
		printf("ERROR: p_img or sz_img is invalid.\n");
		return false;
	}
	if(sz_resize_x<=0 || sz_resize_y<=0 || sz_resize_z<=0)
	{
		printf("ERROR: input new image size is invalid!\n");
		return false;
	}

	//allocate memory
	T *p_img_resize=new T[sz_resize_x*sz_resize_y*sz_resize_z*sz_img[3]]();
	if(!p_img_resize)
	{
		printf("ERROR: Fail to allocate memory for p_img_resize.\n");
		return false;
	}
	T ****p_img_resize_4d=0,****p_img_4d=0;
	if(!new4dpointer(p_img_resize_4d,sz_resize_x,sz_resize_y,sz_resize_z,sz_img[3],p_img_resize) ||
	   !new4dpointer(p_img_4d,sz_img[0],sz_img[1],sz_img[2],sz_img[3],p_img))
	{
		printf("ERROR: Fail to allocate memory for the 4d pointer of image.\n");
		if(p_img_resize_4d) 				{delete4dpointer(p_img_resize_4d,sz_resize_x,sz_resize_y,sz_resize_z,sz_img[3]);}
		if(p_img_4d)		 				{delete4dpointer(p_img_4d,sz_img[0],sz_img[1],sz_img[2],sz_img[3]);}
		if(p_img_resize) 					{delete []p_img_resize;p_img_resize=0;}
		return false;
	}

	//do resize by padding and unpadding
	for(V3DLONG z=0;z<sz_resize_z;z++)
		for(V3DLONG y=0;y<sz_resize_y;y++)
			for(V3DLONG x=0;x<sz_resize_x;x++)
				for(V3DLONG c=0;c<sz_img[3];c++)
				{
					if(x<sz_img[0] && y<sz_img[1] && z<sz_img[2])
						p_img_resize_4d[c][z][y][x]=p_img_4d[c][z][y][x];
				}

	if(p_img_resize_4d) 				{delete4dpointer(p_img_resize_4d,sz_resize_x,sz_resize_y,sz_resize_z,sz_img[3]);}
	if(p_img_4d)		 				{delete4dpointer(p_img_4d,sz_img[0],sz_img[1],sz_img[2],sz_img[3]);}
	if(p_img) 							{delete []p_img;p_img=0;}
	p_img=p_img_resize;					p_img_resize=0;
	sz_img[0]=sz_resize_x;	sz_img[1]=sz_resize_y;	sz_img[2]=sz_resize_z;

	return true;
}
