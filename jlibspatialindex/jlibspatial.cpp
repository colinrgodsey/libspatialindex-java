//
//  jlibspatial.cpp
//  jlibspatialindex
//
//  Created by Colin Godsey on 9/6/14.
//  Copyright (c) 2014 CRG Studios. All rights reserved.
//

#include <spatialindex/SpatialIndex.h>

#include "jlibspatial.h"

using namespace SpatialIndex;

class JNIIVisitor : public IVisitor {
public:
    JNIEnv *env;
    jobject iVistor;
    jclass clazz;
    jmethodID visitNodeMethodID;
    jmethodID visitDataMethodID;
    jmethodID visitData2MethodID;
    
    JNIIVisitor(JNIEnv *_env, jobject _iVistor) {
        env = _env;
        iVistor = _iVistor;
        clazz = env->GetObjectClass(_iVistor);
        
        visitNodeMethodID = env->GetMethodID(clazz, "visitNode", "(J)V");
        visitDataMethodID = env->GetMethodID(clazz, "visitData", "(J)V");
        visitData2MethodID = env->GetMethodID(clazz, "visitData", "([J)V");
        
        /*jmethodID getClassMethodId = env->GetMethodID(clazz, "getClass", "()Ljava/lang/Class;");
        jobject clsObj = env->CallObjectMethod(iVistor, getClassMethodId);
        
        // Now get the class object's class descriptor
        auto cls = env->GetObjectClass(clsObj);
        
        // Find the getName() method on the class object
        jmethodID getNameMethodId = env->GetMethodID(cls, "getName", "()Ljava/lang/String;");
        
        // Call the getName() to get a jstring object back
        jstring strObj = (jstring)env->CallObjectMethod(clsObj, getNameMethodId);
        
        // Now get the c string from the java jstring object
        const char* str = env->GetStringUTFChars(strObj, NULL);*/
        
    }
    
    void visitNode(const INode& n)
	{
        env->CallVoidMethod(iVistor, visitNodeMethodID, reinterpret_cast<jlong>(&n));
	}
    
	void visitData(const IData& d)
	{
        env->CallVoidMethod(iVistor, visitDataMethodID, reinterpret_cast<jlong>(&d));
	}
    
	void visitData(std::vector<const IData*>& values)
	{
        int length = (int)values.size();
        int i = 0;
        auto mapped = new jlong[length];
        
        for( auto it = values.begin(); it != values.end(); ++it ) {
            const IData *d = *it;
            mapped[i] = reinterpret_cast<jlong>(d);
            i++;
        }
        
        auto arr = env->NewLongArray(length);
        
        env->SetLongArrayRegion(arr, 0, length, mapped);
        
        delete [] mapped;
        
        env->CallVoidMethod(iVistor, visitData2MethodID, arr);
	}
};

jlong Java_libspatialindex_LibSpatialIndex_00024Constructors_RTree__Llibspatialindex_IStorageManager_2Llibspatialindex_PropertySet_2
(JNIEnv *env, jobject self, jobject storageManager, jobject propertySet) {
    auto sm = getHandle<IStorageManager>(env, storageManager);
    auto props = getHandle<Tools::PropertySet>(env, propertySet);
    ISpatialIndex *r = RTree::returnRTree(*sm, *props);
    
    return reinterpret_cast<jlong>(r);
}

jlong Java_libspatialindex_LibSpatialIndex_00024Constructors_RTree__Llibspatialindex_IStorageManager_2DIIII
(JNIEnv *env, jobject self, jobject storageManager, jdouble fill, jint indexCap, jint leafCap, jint dimensions, jint variant) {
    auto sm = getHandle<IStorageManager>(env, storageManager);
    id_type *indexIdentifier = new id_type(0); //think this just counts indexes i guess?
    ISpatialIndex *r = RTree::createNewRTree(*sm,
        fill, indexCap, leafCap, dimensions,
        static_cast<RTree::RTreeVariant>(variant), *indexIdentifier);

    return reinterpret_cast<jlong>(r);
}

void Java_libspatialindex_LibSpatialIndex_00024ISpatialIndex_insertData
(JNIEnv *env, jobject _self, jbyteArray _pData, jobject _shape, jlong shapeId) JEXCP_CATCH_START {
    auto self = getHandle<ISpatialIndex>(env, _self);
    auto shape = getHandle<IShape>(env, _shape);
    int len = env->GetArrayLength(_pData);
    auto pData = new jbyte[len];
    env->GetByteArrayRegion(_pData, 0, len, pData);

    self->insertData(len, (const byte*)pData, *shape, shapeId);
    
    delete [] pData;
} JEXCP_CATCH_END;

void Java_libspatialindex_LibSpatialIndex_00024ISpatialIndex_deleteData
(JNIEnv *env, jobject _self, jobject _shape, jlong shapeId) JEXCP_CATCH_START {
    auto self = getHandle<ISpatialIndex>(env, _self);
    auto shape = getHandle<IShape>(env, _shape);
    
    self->deleteData(*shape, shapeId);
} JEXCP_CATCH_END;

void Java_libspatialindex_LibSpatialIndex_00024ISpatialIndex_nearestNeighborQuery
(JNIEnv *env, jobject _self, jint k, jobject _query, jobject _visitor) JEXCP_CATCH_START {
    auto self = getHandle<ISpatialIndex>(env, _self);
    auto query = getHandle<IShape>(env, _query);
    auto visitor = getHandle<IVisitor>(env, _visitor);
    
    self->nearestNeighborQuery(k, *query, *visitor);
} JEXCP_CATCH_END;

void Java_libspatialindex_LibSpatialIndex_00024ISpatialIndex_intersectsWithQuery
(JNIEnv *env, jobject _self, jobject _query, jobject _visitor) JEXCP_CATCH_START {
    auto self = getHandle<ISpatialIndex>(env, _self);
    auto query = getHandle<IShape>(env, _query);
    auto visitor = getHandle<IVisitor>(env, _visitor);
    
    self->intersectsWithQuery(*query, *visitor);
} JEXCP_CATCH_END;

void Java_libspatialindex_LibSpatialIndex_00024ISpatialIndex_containsWhatQuery
(JNIEnv *env, jobject _self, jobject _query, jobject _visitor) JEXCP_CATCH_START {
    auto self = getHandle<ISpatialIndex>(env, _self);
    auto query = getHandle<IShape>(env, _query);
    auto visitor = getHandle<IVisitor>(env, _visitor);
    
    self->containsWhatQuery(*query, *visitor);
} JEXCP_CATCH_END;

void Java_libspatialindex_LibSpatialIndex_00024ISpatialIndex_selfJoinQuery
(JNIEnv *env, jobject _self, jobject _query, jobject _visitor) JEXCP_CATCH_START {
    auto self = getHandle<ISpatialIndex>(env, _self);
    auto query = getHandle<IShape>(env, _query);
    auto visitor = getHandle<IVisitor>(env, _visitor);
    
    self->selfJoinQuery(*query, *visitor);
} JEXCP_CATCH_END;

void Java_libspatialindex_LibSpatialIndex_00024ISpatialIndex_pointLocationQuery
(JNIEnv *env, jobject _self, jobject _query, jobject _visitor) JEXCP_CATCH_START {
    auto self = getHandle<ISpatialIndex>(env, _self);
    auto query = getHandle<IShape>(env, _query);
    auto visitor = getHandle<IVisitor>(env, _visitor);
    
    auto point = dynamic_cast<Point *>(query);
    
    self->pointLocationQuery(*point, *visitor);
} JEXCP_CATCH_END;

DISPOSE_FOR(ISpatialIndex);

jlong Java_libspatialindex_LibSpatialIndex_00024Constructors_MemoryStorageManager
(JNIEnv *env, jobject self) {
    IStorageManager *sm = StorageManager::createNewMemoryStorageManager();
    
    return reinterpret_cast<jlong>(sm);
}

DISPOSE_FOR(IStorageManager);

jlong Java_libspatialindex_LibSpatialIndex_00024Constructors_Point
(JNIEnv *env, jobject self, jdoubleArray arr, jint dims) {
    auto len = env->GetArrayLength(arr);
    auto data = new jdouble[len];
    
    env->GetDoubleArrayRegion(arr, 0, len, data);
    
    IShape *point = new Point(data, dims);
    
    delete [] data;
    
    
    return reinterpret_cast<jlong>(point);
}

DISPOSE_FOR(IShape);

jlong Java_libspatialindex_LibSpatialIndex_00024Constructors_IVisitor
(JNIEnv *env, jobject self, jobject visitor) {
    //TODO: dont unref the visitor ref.
    //there is a cyclic reference between the native and java IVisitor
    //let the java one handle destruction
    auto ref = env->NewWeakGlobalRef(visitor);
    IVisitor *vs = new JNIIVisitor(env, ref);
    
    return reinterpret_cast<jlong>(vs);
}

DISPOSE_FOR(IVisitor);

jlong Java_libspatialindex_LibSpatialIndex_00024IData_getIdentifier
(JNIEnv *env, jobject _self) {
    auto self = getHandle<IData>(env, _self);
    
    return self->getIdentifier();
}

jlong Java_libspatialindex_LibSpatialIndex_00024IData__1getShape
(JNIEnv *env, jobject _self) {
    auto self = getHandle<IData>(env, _self);
    
    IShape *shape;
    
    self->getShape(&shape);
    
    return reinterpret_cast<jlong>(shape);
}

jint Java_libspatialindex_LibSpatialIndex_00024INode_getChildrenCount
(JNIEnv *env, jobject _self) {
    auto self = getHandle<INode>(env, _self);
    
    return self->getChildrenCount();
}

jlong Java_libspatialindex_LibSpatialIndex_00024INode_getChildIdentifier
(JNIEnv *env, jobject _self, jint index) {
    auto self = getHandle<INode>(env, _self);
    
    return self->getChildIdentifier(index);
}

jbyteArray Java_libspatialindex_LibSpatialIndex_00024INode__1getChildData
(JNIEnv *env, jobject _self, jint index) {
    auto self = getHandle<INode>(env, _self);
    
    uint32_t len;
    byte *data;
    
    self->getChildData(index, len, &data);
    
    auto arr = env->NewByteArray(len);
    
    env->SetByteArrayRegion(arr, 0, len, (jbyte *)data);
    
    return arr;
}

jlong Java_libspatialindex_LibSpatialIndex_00024INode__1getChildShape
(JNIEnv *env, jobject _self, jint idx) {
    auto self = getHandle<INode>(env, _self);
    
    IShape *out = NULL;
    
    self->getChildShape(idx, &out);
    
    return reinterpret_cast<jlong>(out);
}

jint Java_libspatialindex_LibSpatialIndex_00024INode_getLevel
(JNIEnv *env, jobject _self) {
    auto self = getHandle<INode>(env, _self);
    
    return self->getLevel();
}

jboolean Java_libspatialindex_LibSpatialIndex_00024INode_isLeaf
(JNIEnv *env, jobject _self) {
    auto self = getHandle<INode>(env, _self);
    
    return self->isLeaf();
}

jboolean Java_libspatialindex_LibSpatialIndex_00024INode_isIndex
(JNIEnv *env, jobject _self) {
    auto self = getHandle<INode>(env, _self);
    
    return self->isIndex();
}

