import java.rmi.server.UnicastRemoteObject;
import java.rmi.RemoteException;
import java.util.Collections;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger; 
import java.util.HashMap;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry; 
import java.util.concurrent.locks.ReadWriteLock;  
import java.util.concurrent.locks.ReentrantReadWriteLock;


public class RMIHistogram implements RemoteHistogram, Binder {
    private final AtomicInteger previousID = new AtomicInteger(); 
    private final Map<Integer, int[]> histogramsMap = Collections.synchronizedMap(new HashMap<>());
    private final Map<Integer, ReadWriteLock> buffer = Collections.synchronizedMap(new HashMap<>());
    

    private RemoteHistogram localStub() throws RemoteException {
        return (RemoteHistogram) UnicastRemoteObject.exportObject(this, 1099);
    }


    private void bindService(String serviceName, RemoteHistogram stub) throws RemoteException {
        Registry registry = LocateRegistry.getRegistry();
        registry.rebind(serviceName, stub);
    }


    @Override
    public void bind(String serviceName) {
        try {
            RemoteHistogram remoteHistogramStub = localStub();
            bindService(serviceName, remoteHistogramStub);
        } 
        
        catch (RemoteException e) {
            e.printStackTrace();
        }
    }
        

    @Override
    public int createHistogram(int bins) throws RemoteException {
        int id = previousID.getAndIncrement();
 
        histogramsMap.put(id, new int[bins]);
        buffer.put(id, new ReentrantReadWriteLock());
        return id;
    }

    @Override
    public void addToHistogram(int histogramID, int value) throws RemoteException {
        ReadWriteLock lock = buffer.get(histogramID);
        if ( lock != null ) {
            lock.writeLock().lock();

            try {
                int[] histogram = histogramsMap.get(histogramID);
                if ( value >= 0 && value < histogram.length && histogram != null ) { 
                    histogram[value] = histogram[value] + 1;

                }
            } 

            finally {
                lock.writeLock().unlock();
            }
        }

 
    }

     
    @Override
    public int[] getHistogram(int histogramID) throws RemoteException {
        ReadWriteLock lock = buffer.get(histogramID);

        if ( lock != null ) {
            lock.readLock().lock();

            try {
                return histogramsMap.get(histogramID);
            }
    
            finally {
                lock.readLock().unlock();
            }
        }
  
        return null;
    } 
}


  

