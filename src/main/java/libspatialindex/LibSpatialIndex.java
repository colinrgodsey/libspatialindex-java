package libspatialindex;

import java.io.File;
import java.util.Vector;

public static class LibSpatialIndex  {
    static {
        String path = "./libjlibspatialindex.jnilib";
        try {
            System.load(new File(path).getCanonicalPath());
        } catch (Exception ex) {
            System.err.println("Failed to load jnilin " + path);
            System.exit(1);
        }
    }

    protected static class Constructors {
        native static public long RTree(IStorageManager storageManager, PropertySet props);

        native static public long RTree(IStorageManager storageManager,
                          double fillFactor, int indexCapacity, int leafCapacity, int dimension,
                          int rtreeVariant);

        native static public long Point(double[] coords, int dimensions);

        native static public long IVisitor(IVisitor visitor);

        native static public long MemoryStorageManager();
    }

    abstract class _AJNIObject {
        protected long instance;
    }

    abstract class AJNIObject extends _AJNIObject {
        abstract protected void dispose();

        protected void deRef() {
            instance = 0;
        }

        @Override
        protected void finalize() {
            if(instance != 0) dispose();
        }

        void close() {
            finalize();
        }
    }

    public class IShape extends AJNIObject {
        public IShape(long inst) {
            instance = inst;
        }
        native protected void dispose();
    }

    public class Point extends IShape {
        public Point(double[] coords) {
            super(Constructors.Point(coords, coords.length));
        }
    }

    abstract class IStorageManager extends AJNIObject {
        native protected void dispose();
    }

    class MemoryStorageManager extends IStorageManager {
        public MemoryStorageManager() {
            instance = Constructors.MemoryStorageManager();
        }
    }

    public class PropertySet {
    }

    public enum RTreeType {
        RV_LINEAR(0),
        RV_QUADRATIC(1),
        RV_RSTAR(2);

        private final int value;

        private RTree(int value) {
            this.value = value;
        }
    }

    class RTree extends ISpatialIndex {
        private RTree(long i) {
            super();
            instance = i;
        }

        public RTree(IStorageManager storageManager, PropertySet props) {
            this(Constructors.RTree(storageManager, props));
        }

        public RTree(IStorageManager storageManager, double fillFactor,
                     int indexCapacity, int leafCapacity, int dimension, int rtreeVariant) {
            this(Constructors.RTree(storageManager, fillFactor, indexCapacity,
                    leafCapacity, dimension, rtreeVariant));
        }
    }


    abstract class ISpatialIndex extends AJNIObject {
        native protected void dispose();

        native void insertData(byte[] pData, IShape shape, long shapeId);
        native void deleteData(IShape shape, long shapeId);

        native void nearestNeighborQuery(int k, IShape query, IVisitor visitor);
        native void intersectsWithQuery(IShape query, IVisitor visitor);
        native void containsWhatQuery(IShape query, IVisitor visitor);
        native void selfJoinQuery(IShape query, IVisitor visitor);
        native void pointLocationQuery(IShape query, IVisitor visitor);

        public void insertData(IShape shape, long shapeId) {
            insertData(new byte[0], shape, shapeId);
        }
    }

    //DO NOT HOLD REFERENCE!
    class INode extends _AJNIObject {
        private INode(long i) {
            super();
            instance = i;
        }

        native int getChildrenCount();
        native long getChildIdentifier(int index);
        native protected byte[] _getChildData(int index);
        native protected long _getChildShape(int index);
        native int getLevel();
        native boolean isIndex();
        native boolean isLeaf();

        public byte[] getChildData(int index) {
            return _getChildData(index);
        }
        public IShape getChildShape(int index) {
            return new IShape(_getChildShape(index));
        }
    }

    //DO NOT HOLD REFERENCE!
    class IData extends _AJNIObject {
        private IData(long i) {
            super();
            instance = i;
        }

        native long getIdentifier();
        native protected long _getShape();

        public IShape getShape() {
            return new IShape(_getShape());
        }
    }

    abstract class IVisitor extends AJNIObject {
        public IVisitor() {
            instance = Constructors.IVisitor(this);
        }

        native protected void dispose();

        //DO NOT HOLD INode or IData references!
        abstract public void visitNode(INode in);
        abstract public void visitData(IData in);
        abstract public void visitData(IData[] in);

        protected void visitNode(long inst) {
            visitNode(new INode(inst));
        }
        protected void visitData(long inst) {
            visitData(new IData(inst));
        }
        protected void visitData(long[] data) {
            IData[] idats = new IData[data.length];
            for(int i = 0 ; i < data.length ; i++)
                idats[i] = new IData(data[i]);

            visitData(idats);
        }

    }

    class DataVisitor extends IVisitor {
        public DataVisitor() {
            super();
        }

        private Vector<Long> builder = new Vector<>();

        public void visitNode(INode in) {}

        public void visitData(IData in) {
            builder.add(in.getIdentifier());
        }

        public void visitData(IData[] in) {
            for(int i = 0 ; i < in.length ; i++) visitData(in[i]);
        }

        public long[] result() {
            long[] out = new long[builder.size()];
            for(int i = 0 ; i < builder.size() ; i++)
                out[i] = builder.get(i).longValue();

            return out;
        }
    }
}
