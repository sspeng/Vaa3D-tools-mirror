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
*
*       Bria, A., et al., (2012) "Stitching Terabyte-sized 3D Images Acquired in Confocal Ultramicroscopy", Proceedings of the 9th IEEE International Symposium on Biomedical Imaging.
*       Bria, A., Iannello, G., "A Tool for Fast 3D Automatic Stitching of Teravoxel-sized Datasets", submitted on July 2012 to IEEE Transactions on Information Technology in Biomedicine.
*
*    3. This material is provided by  the copyright holders (Alessandro Bria  and  Giulio Iannello),  University Campus Bio-Medico and contributors "as is" and any express or implied war-
*       ranties, including, but  not limited to,  any implied warranties  of merchantability,  non-infringement, or fitness for a particular purpose are  disclaimed. In no event shall the
*       copyright owners, University Campus Bio-Medico, or contributors be liable for any direct, indirect, incidental, special, exemplary, or  consequential  damages  (including, but not
*       limited to, procurement of substitute goods or services; loss of use, data, or profits;reasonable royalties; or business interruption) however caused  and on any theory of liabil-
*       ity, whether in contract, strict liability, or tort  (including negligence or otherwise) arising in any way out of the use of this software,  even if advised of the possibility of
*       such damage.
*    4. Neither the name of University  Campus Bio-Medico of Rome, nor Alessandro Bria and Giulio Iannello, may be used to endorse or  promote products  derived from this software without
*       specific prior written permission.
********************************************************************************************************************************************************************************************/

#ifndef CEXPLORERWINDOW_H
#define CEXPLORERWINDOW_H

#include "CPlugin.h"
#include "v3dr_glwidget.h"
#include "renderer_gl2.h"

class teramanager::CExplorerWindow : public QWidget
{
    Q_OBJECT

    private:

        //object members
        V3DPluginCallback2* V3D_env;    //handle of V3D environment
        v3dhandle window;               //generic (void *) handle of the tri-view image window
        XFormWidget* triViewWidget;     //the tri-view image window
        V3dR_GLWidget* view3DWidget;    //3D renderer widget associated to the image window
        V3dR_MainWindow* window3D;      //the window enclosing <view3DWidget>
        CExplorerWindow *next, *prev;   //the next (higher resolution) and previous (lower resolution) <CExplorerWindow> objects
        int volResIndex;                //resolution index of the volume displayed in the current window (see member <volumes> of CImport)
        int volV0, volV1;               //first and last vertical coordinates of the volume displayed in the current window
        int volH0, volH1;               //first and last horizontal coordinates of the volume displayed in the current window
        int volD0, volD1;               //first and last depth coordinates of the volume displayed in the current window
        string title;                   //title of current window
        bool toBeClosed;                //true is the current window is marked is going to be closed

        //inhibiting defeault constructor
        CExplorerWindow();

        /**********************************************************************************
        * Estimates actual image voxel size (in pixels) of the 3D renderer.
        * This depends on the zoom and on the dimensions of the 3D  renderer. This calcula-
        * tion is based on MATLAB curve  fitting results on  a set of measures manually ex-
        * tracted on the renderer.
        ***********************************************************************************/
        float estimateRendererVoxelSize();

        /**********************************************************************************
        * Returns the most  likely 3D  point in the  image that the user is pointing on the
        * renderer at the given location.
        * This is based on the Vaa3D 3D point selection with one mouse click.
        ***********************************************************************************/
        XYZ getRenderer3DPoint(int x, int y) throw (MyException);

        /**********************************************************************************
        * Vaa3D Renderer_gl1 subclass needed to access some protected methods to be used to
        * select a 3D point in the image space given the coordinates of the mouse cursor.
        ***********************************************************************************/
        //---------- feature disabled because the necessary Vaa3D sources can't be included
        /*class myRenderer : Renderer_gl1
        {
            public:

            friend class CExplorerWindow;

            XYZ get3DPoint(int x, int y)
            {
                MarkerPos pos;
                pos.x = x;
                pos.y = y;
                for (int i=0; i<4; i++)
                        pos.view[i] = viewport[i];
                for (int i=0; i<16; i++)
                {
                    pos.P[i]  = projectionMatrix[i];
                    pos.MV[i] = markerViewMatrix[i];
                }
                return this->getCenterOfMarkerPos(pos);
            }
        };*/


    public:

        //CONSTRUCTOR, DECONSTRUCTOR
        CExplorerWindow(V3DPluginCallback2* _V3D_env, int _resIndex, uint8* imgData, int _volV0, int _volV1,
                       int _volH0, int _volH1, int _volD0, int _volD1, CExplorerWindow* _prev);
        ~CExplorerWindow();

        //first window of the multiresolution explorer windows chain
        static CExplorerWindow *first;
        static void uninstance(){if(first){delete first; first=0;}}

        /**********************************************************************************
        * Filters events generated by the 3D rendering window <view3DWidget>
        * We're interested to intercept these events to provide many useful ways to explore
        * the 3D volume at different resolutions without changing Vaa3D code.
        ***********************************************************************************/
        bool eventFilter(QObject *object, QEvent *event);

        /**********************************************************************************
        * Restores the current window and destroys the next <CExplorerWindow>.
        * Called by the next <CExplorerWindow> when the user zooms out and  the lower reso-
        * olution has to be reestabilished.
        ***********************************************************************************/
        void restore();

        /**********************************************************************************
        * Switches to the higher resolution at the given 3D point.
        * Called by the current <CExplorerWindow> when the user zooms in and the higher res-
        * lution has to be loaded.
        ***********************************************************************************/
        void switchToHigherRes(int x, int y, int z);


    public slots:

        /**********************************************************************************
        * Called by the signal emitted by <CVolume> when the associated  operation has been
        * performed. If an exception has occurred in the <CVolume> thread, it is propagated
        * and managed in the current thread (ex != 0).
        ***********************************************************************************/
        void loadingDone(MyException *ex, void* sourceObject);
};

#endif // CEXPLORERWINDOW_H
