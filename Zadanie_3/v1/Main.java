import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

public class Main {
    public static void main(String[] args) {
        try {
           
            LocateRegistry.createRegistry(1099);
            System.out.println("RMI registry ready.");

          
            RMIHistogram server = new RMIHistogram();
            server.bind("HistogramService");

            System.out.println("RMIHistogram server is running");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
