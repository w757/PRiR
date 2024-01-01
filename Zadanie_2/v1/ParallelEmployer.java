
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.atomic.AtomicBoolean;



public class ParallelEmployer implements Employer {

    private OrderInterface orderInterface;
    private final AtomicBoolean exitFound = new AtomicBoolean(false);
    private final AtomicReference<Location> exitLocation = new AtomicReference<>();
    private final ExecutorService executorService = Executors.newCachedThreadPool(); 
    private final Map<Integer, Location> LocationMap = new ConcurrentHashMap<>();
    private final Set<Location> verifiedLocations = ConcurrentHashMap.newKeySet();


    @Override
    public void setOrderInterface(OrderInterface order) {

        this.orderInterface = order;
        this.orderInterface.setResultListener(result -> { 
            
            if (exitFound.get()) { return; }
            Location location = LocationMap.get(result.orderID());

            switch (result.type()) {
                case PASSAGE:
                    if (location != null) { explore(location, result.allowedDirections()); } 
                    break;

                case EXIT:
                    exitFound.set(true);
                    exitLocation.set(location);
                    shutdownExecutor();
                    break;
            }
       });
    }


    @Override
    public Location findExit(Location startLocation, List<Direction> allowedDirections) {
        if (!exitFound.get()) {
            verifiedLocations.add(startLocation);
            LocationMap.put(orderInterface.order(startLocation), startLocation);
            explore(startLocation, allowedDirections);
            awaitCompletion();
        }
        return exitLocation.get();
    }


    private void explore(Location currentLocation, List<Direction> directions) {
        if (exitFound.get()) return;

        directions.forEach(direction -> {
            Location newLocation = direction.step(currentLocation);

            if (verifiedLocations.add(newLocation)) {
                executorService.submit(() -> {
                    int orderId = orderInterface.order(newLocation);
                    LocationMap.put(orderId, newLocation);
                }); 
        }
    });}

    private void shutdownExecutor() { executorService.shutdownNow();}

    private void awaitCompletion() {

        try { if (!executorService.awaitTermination(60, TimeUnit.SECONDS)) {
                shutdownExecutor();
            }

        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}












