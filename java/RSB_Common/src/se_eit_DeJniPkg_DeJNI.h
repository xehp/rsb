/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class se_eit_DeJniPkg_DeJNI */

#ifndef _Included_se_eit_DeJniPkg_DeJNI
#define _Included_se_eit_DeJniPkg_DeJNI
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     se_eit_DeJniPkg_DeJNI
 * Method:    sayHello
 * Signature: (JLjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_se_eit_DeJniPkg_DeJNI_sayHello
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     se_eit_DeJniPkg_DeJNI
 * Method:    loadFile
 * Signature: (Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_se_eit_DeJniPkg_DeJNI_loadFile
  (JNIEnv *, jobject, jstring, jstring);

/*
 * Class:     se_eit_DeJniPkg_DeJNI
 * Method:    saveToFile
 * Signature: (JLjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_se_eit_DeJniPkg_DeJNI_saveToFile
  (JNIEnv *, jobject, jlong, jstring);

#ifdef __cplusplus
}
#endif
#endif