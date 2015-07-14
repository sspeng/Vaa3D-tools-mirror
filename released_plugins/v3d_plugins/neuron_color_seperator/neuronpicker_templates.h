#ifndef NEURONPICKER_TEMPLATES_H
#define NEURONPICKER_TEMPLATES_H

#include <v3d_interface.h>

//assum image has 4 channels, the intensity value is the maximum from first 3 volume, the mask is from the 4th volume
template <class T>
double findBgthrInMask(T *data1D, V3DLONG pagesize, T threshold)
{
    double intens=0;
    double vol_num=0;
    for(V3DLONG vid=0; vid<pagesize; vid++){
        if(data1D[vid+pagesize*3]>threshold){
            T maxval=MAX(data1D[vid],data1D[vid+pagesize]);
            maxval=MAX(maxval,data1D[vid+pagesize*2]);
            intens+=maxval;
            vol_num++;
        }
    }
    return intens/vol_num;
}

template <class T>
void findMaxMinVal(T *data1D, V3DLONG len, V3DLONG & max_ind, T & max_val,  V3DLONG & min_ind, T & min_val)
{
    max_val=data1D[0];
    min_val=data1D[0];
    max_ind=0;
    min_ind=0;
    for(V3DLONG vid=0; vid<len; vid++){
        if(data1D[vid]<min_val){
            min_val=data1D[vid];
            min_ind=vid;
        }
        if(data1D[vid]>max_val){
            max_val=data1D[vid];
            max_ind=vid;
        }
    }

}

template <class T>
void normalizeEachChannelTo255(T * data1Dc, V3DLONG sz_img[4])
{
    V3DLONG page_size = sz_img[0]*sz_img[1]*sz_img[2];
    for(V3DLONG cid=0; cid<sz_img[3]; cid++){
        V3DLONG page = page_size*cid;
        T max_val, min_val;
        V3DLONG max_ind, min_ind;
        findMaxMinVal<T>(data1Dc+page, page_size, max_ind, max_val, min_ind, min_val);

        float scale=(float)(max_val-min_val);
        if(scale>1e-16){
            scale/=255;
            for(V3DLONG vid=page; vid<page+page_size; vid++){
                data1Dc[vid]-=min_val;
                data1Dc[vid]/=scale;
            }
        }else{
            memset(data1Dc+page,0,page_size*sizeof(T));
        }
    }
}

template <class T>
void shift2bitsTo255(T * data1Dc, V3DLONG sz)
{
    for(V3DLONG i=0; i<sz; i++){
        data1Dc[i]/=4;
        data1Dc[i]=data1Dc[i]<255?data1Dc[i]:255;
    }
}

#endif // NEURONPICKER_TEMPLATES_H
