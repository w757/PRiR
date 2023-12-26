import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;

public class ParallelEmployer implements Employer {

    private OrderInterface orderInterface;
    private final Set<Location> exploredLocations = ConcurrentHashMap.newKeySet();
    private final Map<Integer, Location> orderToLocationMap = new ConcurrentHashMap<>();
    private final ExecutorService executorService = Executors.newCachedThreadPool();
    private final AtomicBoolean exitFound = new AtomicBoolean(false);
    private final AtomicReference<Location> exitLocation = new AtomicReference<>();

    @Override
    public void setOrderInterface(OrderInterface order) {
        this.orderInterface = order;
        this.orderInterface.setResultListener(result -> {
            if (exitFound.get()) {
                return;
            }

            Location location = orderToLocationMap.get(result.orderID());
            if (location != null && result.type() == LocationType.PASSAGE) {
                explore(location, result.allowedDirections());
            } else if (result.type() == LocationType.EXIT) {
                exitFound.set(true);
                exitLocation.set(location);
                shutdownExecutor();
            }
        });
    }

    @Override
    public Location findExit(Location startLocation, List<Direction> allowedDirections) {
        if (!exitFound.get()) {
            exploredLocations.add(startLocation);
            orderToLocationMap.put(orderInterface.order(startLocation), startLocation);
            explore(startLocation, allowedDirections);
            awaitCompletion();
        }
        return exitLocation.get();
    }

    private void explore(Location currentLocation, List<Direction> directions) {
        if (exitFound.get()) return;

        for (Direction direction : directions) {
            Location newLocation = direction.step(currentLocation);
            if (exploredLocations.add(newLocation)) {
                executorService.submit(() -> {
                    int orderId = orderInterface.order(newLocation);
                    orderToLocationMap.put(orderId, newLocation);
                });
            }
        }
    }

    private void shutdownExecutor() {
        executorService.shutdownNow();
    }

    private void awaitCompletion() {
        try {
            if (!executorService.awaitTermination(60, TimeUnit.SECONDS)) {
                shutdownExecutor();
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}
