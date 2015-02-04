//
//  jlibspatial.h
//  jlibspatialindex
//
//  Created by Colin Godsey on 9/6/14.
//  Copyright (c) 2014 CRG Studios. All rights reserved.
//

#ifndef __jlibspatialindex__jlibspatial__
#define __jlibspatialindex__jlibspatial__

#include <iostream>
#include "jni_libspatialindex.h"

#define DISPOSE_FOR(XX) \
void Java_libspatialindex_ ## XX ## _dispose(JNIEnv *env, jobject obj) { \
    auto nativeObject = getHandle<XX>(env, obj); \
    setHandle(env, obj, 0); \
    delete nativeObject; \
}

#define JEXCP_CATCH_START \
try

#define JEXCP_CATCH_END \
 catch(const Tools::IllegalArgumentException &ex) { \
auto ex2 = (Tools::IllegalArgumentException)ex; \
NewJavaException(env, "java/lang/IllegalArgumentException", ex2.what().data()); \
} catch(const std::exception& e) { \
NewJavaException(env, "java/lang/Error", e.what()); \
}

struct ThrownJavaException : std::runtime_error {
    ThrownJavaException() :std::runtime_error("") {}
    ThrownJavaException(const std::string& msg ) :std::runtime_error(msg) {}
};

struct NewJavaException : public ThrownJavaException {
    NewJavaException(JNIEnv * env, const char* type="", const char* message="")
    :ThrownJavaException(type+std::string(" ")+message)
    {
        jclass newExcCls = env->FindClass(type);
        if (newExcCls != NULL)
            env->ThrowNew(newExcCls, message);
        //if it is null, a NoClassDefFoundError was already thrown
    }
};

jfieldID getHandleField(JNIEnv *env, jobject obj)
{
    jclass c = env->GetObjectClass(obj);
    // J is the type signature for long:
    return env->GetFieldID(c, "instance", "J");
}

template <typename T>
T *getHandle(JNIEnv *env, jobject obj)
{
    jlong handle = env->GetLongField(obj, getHandleField(env, obj));
    return reinterpret_cast<T *>(handle);
}

void setHandle(JNIEnv *env, jobject obj, void *t)
{
    jlong handle = reinterpret_cast<jlong>(t);
    env->SetLongField(obj, getHandleField(env, obj), handle);
}

#endif /* defined(__jlibspatialindex__jlibspatial__) */
