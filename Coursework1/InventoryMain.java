public class InventoryMain
{
    public static void main(String[] args)
    {
        int firstArg,secondArg,thirdArg,i=0;
        if(args.length == 3)
        {
            //if there are too many arguments it will print an error message
            try
            {
                // converts the first 3 numbers to integers
                // if they are not integers it will print an error statement and exit the program
                // then if the bugflag(3rd number) is equal to 0 or 1 
                // it will create a new object of MultiThreading called myThing and run it
                firstArg = Integer.parseInt(args[0]);
                secondArg = Integer.parseInt(args[1]);
                thirdArg = Integer.parseInt(args[2]);
                if(thirdArg == 1 || thirdArg ==0)
                {
                    MultiThreading myThing = new MultiThreading(firstArg,secondArg,thirdArg);
                    myThing.run();
                }
                else{
                    System.err.println("Bug Flag must be 1 or 0");
                    System.exit(1);
                }
            }
            catch(NumberFormatException e)
            {
                System.err.println("Argument number "+ (i+1)+ " must be an integer");
                System.exit(1);
            }
        }
        if(args.length>3)
        {
            System.out.println("Too many arguments");
        }
        if(args.length<3)
        {
            System.out.println("Not enough arguments");
        }
    }
}