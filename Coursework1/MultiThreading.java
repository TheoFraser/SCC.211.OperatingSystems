public class MultiThreading
{
    public int add,sub,bugFlag,inventorySize=0;
    public MultiThreading(int add,int sub,int bugFlag)
    {
        this.add=add;
        this.sub=sub;
        this.bugFlag=bugFlag;
    }
    public void run()
    {
        //System.out.println(addThread);
        //System.out.println(subThread);
        //System.out.println(binaryThread);
        int totalThreads = add+sub; //this calculates the totalThreads required
        InventoryTotal total = new InventoryTotal(0,totalThreads); //creates a new instance of Inventory Total
        //CreateThread addThread = new CreateThread(total,1,bugFlag); //creates a new object of CreateThread called addThread
        //CreateThread subThread = new CreateThread(total,-1,bugFlag); //creates a new object of CreateThread called subThread
        //CreateThread finalThread = new CreateThread();

        //this for loop creates all the add threads
        for(int i =0; i<add;i++)
        {
            CreateThread addThread = new CreateThread(total,1,bugFlag); //creates a new object of CreateThread called addThread 
            Thread myThread = new Thread(addThread);
            myThread.start();
        }

        //this for loop creates all the sub threads
        for(int i =0; i<sub;i++)
        {
            CreateThread subThread = new CreateThread(total,-1,bugFlag);
            Thread myThread = new Thread(subThread); //creates a new object of CreateThread called subThread
            myThread.start();
        }
    }
}