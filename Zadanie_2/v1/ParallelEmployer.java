import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ParallelEmployer implements Employer {

    private OrderInterface orderInterface;
    private final Set<Location> exploredLocations = ConcurrentHashMap.newKeySet();
    private final ExecutorService executorService = Executors.newCachedThreadPool();

    @Override
    public void setOrderInterface(OrderInterface order) {
        this.orderInterface = order;
    }

    @Override
    public Location findExit(Location startLocation, List<Direction> allowedDirections) {
        exploredLocations.add(startLocation);
        explore(startLocation, allowedDirections);
        try {
            executorService.shutdown();
            while (!executorService.isTerminated()) {
                Thread.sleep(100); // Uspienie wątku, gdy czeka na wyniki
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
        return startLocation; // Zwraca lokację wyjścia, należy zaktualizować logikę
    }

    private void explore(Location currentLocation, List<Direction> directions) {
        for (Direction direction : directions) {
            Location newLocation = direction.step(currentLocation);
            if (exploredLocations.add(newLocation)) { // Sprawdzenie, czy lokacja nie była już eksplorowana
                executorService.submit(() -> {
                    int orderId = orderInterface.order(newLocation);
                    // logika przetwarzania wyniku zlecenia
                });
            }
        }
    }

    public void setResultListener(ResultListener listener) {
        // Implementacja metody ustawiającej listenera wyników
    }
}
