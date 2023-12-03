import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

public class RMIHistogramClient {
    public static void main(String[] args) {
        try {
            Registry registry = LocateRegistry.getRegistry("localhost");
            RemoteHistogram histogramService = (RemoteHistogram) registry.lookup("HistogramService");

            int histogramId = histogramService.createHistogram(10);
            
            // Klient 1
            Thread client1 = new Thread(() -> {
                for (int i = 0; i < 10; i++) {
                    try {
                        histogramService.addToHistogram(histogramId, i);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }
                }
            });

            // Klient 2
            Thread client2 = new Thread(() -> {
                for (int i = 0; i < 10; i++) {
                    try {
                        histogramService.addToHistogram(histogramId, i);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }
                }
            });

            // Klient 3
            Thread client3 = new Thread(() -> {
                for (int i = 0; i < 10; i++) {
                    try {
                        histogramService.addToHistogram(histogramId, i);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }
                }
            });

            // Klient 4
            Thread client4 = new Thread(() -> {
                for (int i = 0; i < 10; i++) {
                    try {
                        histogramService.addToHistogram(histogramId, i);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }
                }
            });

            // Uruchomienie klientów
            client1.start();
            client2.start();
            client3.start();
            client4.start();

            // Oczekiwanie na zakończenie klientów
            client1.join();
            client2.join();
            client3.join();
            client4.join();


            // Wyświetlanie histogramu
            int[] histogram = histogramService.getHistogram(histogramId);
            for (int i = 0; i < histogram.length; i++) {
                System.out.println("Bin " + i + ": " + histogram[i]);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
