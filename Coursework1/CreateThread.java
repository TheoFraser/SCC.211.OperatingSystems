public class CreateThread implements Runnable
{
    //this class is what each thread does
    //if the bugFlag is 0 it will add the value to the total correctly with synchronized
    //if the bugFlag is 1 it will add the value to the total without synchronized
    public int sign,bugFlag;
    public InventoryTotal total;
    public CreateThread(InventoryTotal total,int sign,int bugFlag)
    {
        this.sign=sign;
        this.total=total;
        this.bugFlag=bugFlag;
    }
    @Override
    public void run()
    {
        if(bugFlag==0)
        {
            total.updateSynchro(sign);
        }
        if(bugFlag==1)
        {
            total.updateNonSynchro(sign);
        }
    }
}