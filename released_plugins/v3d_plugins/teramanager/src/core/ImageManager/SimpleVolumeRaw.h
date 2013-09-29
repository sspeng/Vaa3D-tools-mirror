//------------------------------------------------------------------------------------------------
// Copyright (c) 2012  Alessandro Bria and Giulio Iannello (University Campus Bio-Medico of Rome).  
// All rights reserved.
//------------------------------------------------------------------------------------------------

/*******************************************************************************************************************************************************************************************
*    LICENSE NOTICE
********************************************************************************************************************************************************************************************
*    By downloading/using/running/editing/changing any portion of codes in this package you agree to this license. If you do not agree to this license, do not download/use/run/edit/change
*    this code.
********************************************************************************************************************************************************************************************
*    1. This material is free for non-profit research, but needs a special license for any commercial purpose. Please contact Alessandro Bria at a.bria@unicas.it or Giulio Iannello at 
*       g.iannello@unicampus.it for further details.
*    2. You agree to appropriately cite this work in your related studies and publications.
*    3. This material is provided by  the copyright holders (Alessandro Bria  and  Giulio Iannello),  University Campus Bio-Medico and contributors "as is" and any express or implied war-
*       ranties, including, but  not limited to,  any implied warranties  of merchantability,  non-infringement, or fitness for a particular purpose are  disclaimed. In no event shall the
*       copyright owners, University Campus Bio-Medico, or contributors be liable for any direct, indirect, incidental, special, exemplary, or  consequential  damages  (including, but not 
*       limited to, procurement of substitute goods or services; loss of use, data, or profits;reasonable royalties; or business interruption) however caused  and on any theory of liabil-
*       ity, whether in contract, strict liability, or tort  (including negligence or otherwise) arising in any way out of the use of this software,  even if advised of the possibility of
*       such damage.
*    4. Neither the name of University  Campus Bio-Medico of Rome, nor Alessandro Bria and Giulio Iannello, may be used to endorse or  promote products  derived from this software without
*       specific prior written permission.
********************************************************************************************************************************************************************************************/

# ifndef _SIMPLE_VOLUME_RAW_H
# define _SIMPLE_VOLUME_RAW_H

# include "VirtualVolume.h" 

# define SIMPLE_RAW_FORMAT "SimpleRaw" 

//FORWARD-DECLARATIONS
class  StackRaw;

class SimpleVolumeRaw : public VirtualVolume {
private:
	uint16 N_ROWS, N_COLS;		//dimensions (in stacks) of stacks matrix along VH axes
    StackRaw ***STACKS;			//2-D array of <Stack*>

	void init ( );

	// iannello returns the number of channels of images composing the volume
	void initChannels ( ) throw (MyException);

public:
	SimpleVolumeRaw(const char* _root_dir)  throw (MyException);

	~SimpleVolumeRaw(void);

	REAL_T *loadSubvolume_to_REAL_T(int V0=-1,int V1=-1, int H0=-1, int H1=-1, int D0=-1, int D1=-1)  throw (MyException);

    uint8 *loadSubvolume_to_UINT8(int V0=-1,int V1=-1, int H0=-1, int H1=-1, int D0=-1, int D1=-1, int *channels=0, int ret_type=IM_DEF_IMG_DEPTH) throw (MyException);
};		

# endif // _SIMPLE_VOLUME
