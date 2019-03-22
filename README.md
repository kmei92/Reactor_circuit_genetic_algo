# Team Platinum - Optimal Mineral Recovery Using Genetic Algorithms
Applied Computational Science and Engineering (ACSE-4): Group Project 3

## **Synopsis**
This project simulates the genetic algorithms with both serial and parallel programming, and achieves the optimal mineral recovery.

The key parts of the simulation program are:

- The Genetic Algorithm (including a defined fitness number calculation method)
- Circuit Simulation (able to caculate the mass balance over all the units)
- Circuit Validity (check if the curcuit is valid, and if the output result is converged)


The key outputs from the simulation program are:

- Optimal curcuit value (serial and parallel).
- Optimal circuit with diffrerent mutation rate and number of child vectors.
- Optimal circuit with changing the penalty price to zero.


## **Installation**

The following software and modules are required to be installed:
- Visual Studio Community 2017
-	Jupyter Notebook (Included in Anaconda)
    - Matplotlib.pyplot, matplotlib >= 2.2.2
    -	Numpy >= 1.14.3
    -	Graphviz >= 1.0.0
-	IPython Kernel (Included in Anaconda)

Clone this repository to local folder, choose one of two ways to allocate the source and header files before compiling and running:
- Gather all source files in the src folder and header files in the includes folder from the cloned local repository and then put them inside the visual studio project folder which contains the .vcxproj file.
- Gather all source files in the src folder and put them in the includes folder from the cloned local repository. 
1.	Enter the visual studio project.
2.	Right click on the project in the Solution Explorer and go to Properties, then click on the C++ Directories and choose General
3.	Edit the Additional Include Directories, add the path of the modified includes folder, then click Apply in the bottom right corner.
- Choose either method listed above, then right click the Source Files folder inside visual studio, choose add -> existing item to add all source(.cpp) files 
- Right click the Header Files folder inside visual studio project, choose add -> existing item and add all header(.h) files.
To support the parallel version of the genetic algorithm simulator, the following steps are required:

Install Microsoft MPI: the required files can be found at https://docs.microsoft.com/en-us/message-passing-interface/microsoft-mpi. Click on Downloads to download and install both files.
-	Add the location of the downloaded files to the system PATH. Open the Window’s System Properties dialog box and click on Environment Variables and click edit for the Path variable and add “C:\MS_mpi\Bin” (assuming the default install location).
-	Open Visual Studio to create a new project, right click on the project in the Solution Explorer and go to Properties, then click on VC++ Directories and (again assuming the MPI SDK is installed in the default directory) add:
    -	“C\MS_mpi\Include” to the Included Directories
    -	“C\MS_mpi\Lib\x64” or “C\MS_mpi\Lib\x86” to the Library Directories (depending on whether to compile the file as 32 or 64 bit, respectively)
-	Then click on Linker and choose Input, add the “msmpi.lib” to the Additional Dependencies.
Ensure that the project contains the above linkage before compiling and running.


## **Structure**

### **main():**

- This is the main function which initilaizes the parameters and call the genetic algorithm, circuit simulator and check validity function.

### **Genetic Algorithms:**

**RollingDice():**

- Randomly decide if the unit needs to do crossover
- Compare the random number with the initialized crossover rate, return true or false for crossover or not crossover

**mutation():**

- This function goes through a vector and randomly (based on the gene change rate) if each number inside the vector mutate to a different number

**swapping_parent():**

- Swap the old and new parents for the next iteration

**run_genetic_algorithm():**

- This is the main genetic algorithm function that executes and produces the solution circuit value.

### **Circuit Simulation:**

**Evaluate_Circuit():**

- This is the evaluate circuit function. The result will be varied with changing the iteration times.
- It returns the performance (revenue) of a circuit vector.

### **Circuit Validity:**

**Check_Validity():**

- The function defined the conditions for a valid circuit value, any circuit cannot meet all the conditions will be markedd as an invalid circuit.

**Mark_Units():**

- This is the recursive function that enters a circuit vector and marks every separation unit if needed.

## **Output**

## **Roadmap**

-	Group members divided into two groups 

    -	Group Genetic Algorithm: Sanaz, keer and Ming

    -	Group Circuit Simulation: Yusuf, Sean and Michael


-	Daily schedule:

    -	9am resumption

    -	12:30pm – 2pm lunch


Timeline

•	Genetic Algorithm code: Monday 

•	Circuit simulation code: Tuesday 

•	Validity checking function: Tuesday

•	Travis Testing functions: Wednesday

•	Measuring rate of convergence of original code? Wednesday

•	Investigate how probability of crossing selected parents (0.8 – 1) affects rate of convergence. Wednesday

•	Investigate how rate at which mutations are introduced (<1% probability) affect rate of convergence. Thursday

•	Parallelisation using OpenMP: Thursday

•	Rate of convergence affected by the number of child vectors used. Thursday

•	Rate of convergence affected by mutation rate. Thursday

•	Code documentation and auto-documentation: Thursday

•	Preparation of report and github readme file: Thursday

•	Preparation of slides and presentation: Friday

  # Contributors

  - [**Professor Stephen Neethling**](https://www.imperial.ac.uk/people/s.neethling)
  - [**Platinum group**](https://github.com/msc-acse/acse-4-project-3-platinum)
