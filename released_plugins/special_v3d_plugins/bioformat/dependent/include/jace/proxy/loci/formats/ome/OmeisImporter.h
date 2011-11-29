#ifndef JACE_PROXY_LOCI_FORMATS_OME_OMEISIMPORTER_H
#define JACE_PROXY_LOCI_FORMATS_OME_OMEISIMPORTER_H

#ifndef JACE_OS_DEP_H
#include "jace/os_dep.h"
#endif

#ifndef JACE_NAMESPACE_H
#include "jace/namespace.h"
#endif

#ifndef JACE_JOBJECT_H
#include "jace/proxy/JObject.h"
#endif

#ifndef JACE_JARRAY_H
#include "jace/JArray.h"
#endif

#ifndef JACE_JFIELD_PROXY_H
#include "jace/JFieldProxy.h"
#endif

#ifndef JACE_JCLASSIMPL_H
#include "jace/JClassImpl.h"
#endif

/**
 * The super class for this class.
 *
 */
#ifndef JACE_PROXY_JAVA_LANG_OBJECT_H
#include "jace/proxy/java/lang/Object.h"
#endif

/**
 * Classes which this class is fully dependent upon.
 *
 */
#ifndef JACE_PROXY_JAVA_LANG_STRING_H
#include "jace/proxy/java/lang/String.h"
#endif

#ifndef JACE_TYPES_JINT_H
#include "jace/proxy/types/JInt.h"
#endif

/**
 * Forward declarations for the classes that this class uses.
 *
 */
BEGIN_NAMESPACE_4( jace, proxy, java, lang )
class String;
END_NAMESPACE_4( jace, proxy, java, lang )

BEGIN_NAMESPACE_3( jace, proxy, types )
class JBoolean;
END_NAMESPACE_3( jace, proxy, types )

BEGIN_NAMESPACE_5( jace, proxy, loci, formats, ome )
class OmeisException;
END_NAMESPACE_5( jace, proxy, loci, formats, ome )

BEGIN_NAMESPACE_4( jace, proxy, java, io )
class IOException;
END_NAMESPACE_4( jace, proxy, java, io )

BEGIN_NAMESPACE_4( jace, proxy, java, util )
class Hashtable;
END_NAMESPACE_4( jace, proxy, java, util )

BEGIN_NAMESPACE_3( jace, proxy, types )
class JInt;
END_NAMESPACE_3( jace, proxy, types )

BEGIN_NAMESPACE_4( jace, proxy, loci, formats )
class FormatException;
END_NAMESPACE_4( jace, proxy, loci, formats )

BEGIN_NAMESPACE_3( jace, proxy, types )
class JVoid;
END_NAMESPACE_3( jace, proxy, types )

BEGIN_NAMESPACE_5( jace, proxy, loci, formats, ome )

/**
 * The Jace C++ proxy class for loci.formats.ome.OmeisImporter.
 * Please do not edit this class, as any changes you make will be overwritten.
 * For more information, please refer to the Jace Developer's Guide.
 *
 */
class OmeisImporter : public virtual ::jace::proxy::java::lang::Object
{
public: 

/**
 * OmeisImporter
 *
 */
OmeisImporter();

/**
 * OmeisImporter
 *
 */
OmeisImporter( ::jace::proxy::types::JBoolean p0 );

/**
 * printVersion
 *
 */
void printVersion();

/**
 * testIds
 *
 */
void testIds( ::jace::JArray< ::jace::proxy::types::JInt > p0 );

/**
 * importIds
 *
 */
void importIds( ::jace::JArray< ::jace::proxy::types::JInt > p0 );

/**
 * getLocalFilePath
 *
 */
::jace::proxy::java::lang::String getLocalFilePath( ::jace::proxy::types::JInt p0 );

/**
 * getFileInfo
 *
 */
::jace::proxy::java::util::Hashtable getFileInfo( ::jace::proxy::types::JInt p0 );

/**
 * newPixels
 *
 */
::jace::proxy::types::JInt newPixels( ::jace::proxy::types::JInt p0, ::jace::proxy::types::JInt p1, ::jace::proxy::types::JInt p2, ::jace::proxy::types::JInt p3, ::jace::proxy::types::JInt p4, ::jace::proxy::types::JInt p5, ::jace::proxy::types::JBoolean p6, ::jace::proxy::types::JBoolean p7 );

/**
 * isLittleEndian
 *
 */
::jace::proxy::types::JBoolean isLittleEndian();

/**
 * getLocalPixelsPath
 *
 */
::jace::proxy::java::lang::String getLocalPixelsPath( ::jace::proxy::types::JInt p0 );

/**
 * finishPixels
 *
 */
::jace::proxy::types::JInt finishPixels( ::jace::proxy::types::JInt p0 );

/**
 * getPixelsSHA1
 *
 */
::jace::proxy::java::lang::String getPixelsSHA1( ::jace::proxy::types::JInt p0 );

/**
 * main
 *
 */
static void main( ::jace::JArray< ::jace::proxy::java::lang::String > p0 );

/**
 * The following methods are required to integrate this class
 * with the Jace framework.
 *
 */
OmeisImporter( jvalue value );
OmeisImporter( jobject object );
OmeisImporter( const OmeisImporter& object );
OmeisImporter( const NoOp& noOp );
virtual const JClass& getJavaJniClass() const throw ( ::jace::JNIException );
static const JClass& staticGetJavaJniClass() throw ( ::jace::JNIException );
};

END_NAMESPACE_5( jace, proxy, loci, formats, ome )

BEGIN_NAMESPACE( jace )

#ifndef PUT_TSDS_IN_HEADER
  template <> ElementProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>::ElementProxy( jarray array, jvalue element, int index );
  template <> ElementProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>::ElementProxy( const jace::ElementProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>& proxy );
#else
  template <> inline ElementProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>::ElementProxy( jarray array, jvalue element, int index ) : 
    ::jace::proxy::loci::formats::ome::OmeisImporter( element ), Object( NO_OP ), mIndex( index )
  {
    JNIEnv* env = ::jace::helper::attach();
    parent = static_cast<jarray>( ::jace::helper::newGlobalRef( env, array ) );
  }
  template <> inline ElementProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>::ElementProxy( const jace::ElementProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>& proxy ) : 
    ::jace::proxy::loci::formats::ome::OmeisImporter( proxy.getJavaJniObject() ), Object( NO_OP ), mIndex( proxy.mIndex )
  {
    JNIEnv* env = ::jace::helper::attach();
    parent = static_cast<jarray>( ::jace::helper::newGlobalRef( env, proxy.parent ) );
  }
#endif

#ifndef PUT_TSDS_IN_HEADER
  template <> JFieldProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>::JFieldProxy( jfieldID fieldID_, jvalue value, jobject parent_ );
  template <> JFieldProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>::JFieldProxy( jfieldID fieldID_, jvalue value, jclass parentClass_ );
  template <> JFieldProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>::JFieldProxy( const ::jace::JFieldProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>& object );
#else
  template <> inline JFieldProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>::JFieldProxy( jfieldID fieldID_, jvalue value, jobject parent_ ) : 
    ::jace::proxy::loci::formats::ome::OmeisImporter( value ), Object( NO_OP ), fieldID( fieldID_ )
  {
    JNIEnv* env = ::jace::helper::attach();

    if ( parent_ )
      parent = ::jace::helper::newGlobalRef( env, parent_ );
    else
      parent = parent_;

    parentClass = 0;
  }
  template <> inline JFieldProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>::JFieldProxy( jfieldID fieldID_, jvalue value, jclass parentClass_ ) : 
    ::jace::proxy::loci::formats::ome::OmeisImporter( value ), Object( NO_OP ), fieldID( fieldID_ )
  {
    JNIEnv* env = ::jace::helper::attach();

    parent = 0;
    parentClass = static_cast<jclass>( ::jace::helper::newGlobalRef( env, parentClass_ ) );
  }
  template <> inline JFieldProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>::JFieldProxy( const ::jace::JFieldProxy< ::jace::proxy::loci::formats::ome::OmeisImporter>& object ) : 
    ::jace::proxy::loci::formats::ome::OmeisImporter( object.getJavaJniValue() ), Object( NO_OP )
  {
    fieldID = object.fieldID; 

    if ( object.parent )
    {
      JNIEnv* env = ::jace::helper::attach();
      parent = ::jace::helper::newGlobalRef( env, object.parent );
    }
    else
      parent = 0;

    if ( object.parentClass )
    {
      JNIEnv* env = ::jace::helper::attach();
      parentClass = static_cast<jclass>( ::jace::helper::newGlobalRef( env, object.parentClass ) );
    }
    else
      parentClass = 0;
  }
#endif

END_NAMESPACE( jace )

#endif // #ifndef JACE_PROXY_LOCI_FORMATS_OME_OMEISIMPORTER_H

