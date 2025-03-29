import java.lang.Math;  
public class InventoryTotal
{
    private int total = 0;
    private int threadsFin=0;
    private int totalThreads;
    int temp=0;
    public InventoryTotal(int startTotal,int totalThreads)
    {
        //
        this.totalThreads=totalThreads;
        total = startTotal;
    }
    public void updateSynchro(int amount)
    {
        // synchronize means that only one thread can use the code at a time
        // so only thread can increase total by the amount given at the start
        // then print the new value of total add when all threads are finished it will print the final total
        synchronized(this)
        {
            total=total+amount;
            //System.out.println("total amount"+total);
            threadsFin=threadsFin+1;
            if(amount==1)
            {
                System.out.println("Added. Inventory size = "+total);
                //System.out.println("Threads finsihed = "+threadsFin);
            }
            if(amount ==-1)
            {
                System.out.println("Removed. Inventory size = "+total);
                //System.out.println("Threads finsihed = "+threadsFin);
            }
            if(threadsFin==totalThreads)
            {
                System.out.println("Final inventory size = "+total);
            }
        }
    }
    public void updateNonSynchro(int amount)
    {
        
        //System.out.println("old temp is "+temp );
        temp = total;
        temp = temp + amount;
        //System.out.println("Added. Inventory size = "+temp);
        if(amount==1)
            {
                System.out.println("Added. Inventory size = "+temp);
                //System.out.println("Threads finsihed = "+threadsFin);
            }
            if(amount ==-1)
            {
                System.out.println("Removed. Inventory size = "+temp);
                //System.out.println("Threads finsihed = "+threadsFin);
            }
        try
        {
            Thread.sleep(100);
        }catch(InterruptedException e)
        {
            e.printStackTrace();
        }
        total = temp;
        //System.out.println("new temp is "+temp );
        //System.out.println(total);
            synchronized(this)
            {
                 //System.out.println("Removed. Inventory size = "+total);
                threadsFin=threadsFin+1;
            }
            if(threadsFin==totalThreads)
            {
                System.out.println("Final inventory size = "+total);
            }
    }
}