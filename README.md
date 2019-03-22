# Team Platinum - Optimal Mineral Recovery Using Genetic Algorithms
Applied Computational Science and Engineering (ACSE-4): Group Project 3

Team Member: Yusuf Falola, Xianzheng Li, Keer Mei, Sanaz Salati, Mingrui Zhang

## **Synopsis**
This project implements a genetic algorithm with both serial and parallel programming, to obtain optimal circuit configurations for industrial separation units.

The key components of the program are:

- The Genetic Algorithm (including a defined fitness number calculation method)
- Circuit Simulation (able to caculate the mass balance over all the units)
- Circuit Validity (checks if the curcuit is valid, and if the output result is converged)


The key outputs from the simulation program are:

- Optimal curcuit vector (serial and parallel).
- Optimal circuit with diffrerent mutation rate and number of child vectors.
- Optimal circuit with changing the cost to profit ratios.


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

- This is the main function which initilaizes the parameters and call the genetic algorithm, circuit simulator and check validity functions.

### **Genetic Algorithms Functions:**

**RollingDice():**

- Randomly decides if the unit will crossover
- Compare the random number with the initialized crossover rate, return true or false for crossover or not crossover

**mutation():**

- This function goes through a vector and randomly (based on the gene change rate) decides if each number inside the vector will mutate to a different number

**swapping_parent():**

- Swap the old and new parents for the next iteration

**run_genetic_algorithm():**

- This is the main genetic algorithm function that executes and produces the solution circuit vector.

### **Circuit Simulation:**

**Evaluate_Circuit():**

- This is the evaluate circuit function.
- It returns the performance (revenue) of a circuit vector.

### **Circuit Validity:**

**Check_Validity():**

- The function tests the conditions for a valid circuit. Any circuit that cannot meet all the conditions will be markedd as an invalid circuit.

**Mark_Units():**

- This is a recursive function that enters a circuit vector and marks every separation unit if they are seen.

**NOTE:** 
- Full html auto-documentation, using doxygen, of the three cpp files can be found in the folders;
- circuit_simulation_doc ---> _circuit__simulator_8cpp.html
- Circuit_Validity_Doc ---> _c_circuit_8cpp.html
- GA_documentation ---> html ---> _genetic_algorithm_8cpp.html

## **Output**
### **Main Output**
- Circuit Configuration with 5 units and iteration steps of 10000.
![unit_5](https://user-images.githubusercontent.com/43985789/54815379-bf95b200-4c89-11e9-8623-708931be99c1.PNG)

- Circuit Configuration with 10 units and iteration steps of 10000.
![unit_10](https://user-images.githubusercontent.com/43985789/54815376-befd1b80-4c89-11e9-81d8-c03e859f7007.PNG)

- Circuit Configuration with 15 units and iteration steps of 10000.
![Unit_15](https://user-images.githubusercontent.com/43985789/54815377-befd1b80-4c89-11e9-94c6-d3bb9268ebe2.PNG)

### **Output with Same Price & Different Penalty Value**
- Price is 100, Penalty is 200 (optimal circuit configuration)
<img width="1334" alt="cost200" src="https://user-images.githubusercontent.com/43985789/54817018-bdcded80-4c8d-11e9-9957-bae29db8fa05.png">

- Price is 100, Penalty is 0 (extreme condition for highest purity)
<img width="1334" alt="cost0" src="https://user-images.githubusercontent.com/43985789/54817020-be668400-4c8d-11e9-8a7d-89971213512f.png">

- Price is 100, Penalty is 5000 (extreme condition for most recycle times)
<img width="1337" alt="cost5000" src="https://user-images.githubusercontent.com/43985789/54817021-be668400-4c8d-11e9-8ca6-4fa8ea263489.png">


# Contributors

- [**Professor Stephen Neethling**](https://www.imperial.ac.uk/people/s.neethling)
- [**Platinum group**](https://github.com/msc-acse/acse-4-project-3-platinum)
