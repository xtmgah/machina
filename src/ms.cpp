/*
 * ms.cpp
 *
 *  Created on: 3-jul-2017
 *      Author: M. El-Kebir
 */

#include "utils.h"
#include "frequencymatrix.h"
#include "msenumeration.h"
#include <fstream>
#include <lemon/arg_parser.h>
#include "migrationtree.h"

int main(int argc, char** argv)
{
  std::string primary;
  std::string filenameColorMap;
  std::string migrationTreeFile;
  std::string outputDirectory;
  bool outputSearchGraph = false;
  int limit = -1;
  
  lemon::ArgParser ap(argc, argv);
  ap.refOption("c", "Color map file", filenameColorMap)
    .refOption("g", "Output search graph", outputSearchGraph)
    .refOption("o", "Output prefix" , outputDirectory)
    .refOption("p", "Primary tumor", primary, true)
   .refOption("l", "Maxim number of mutation trees to enumerate (default: -1, unlimited)" , limit)
    .refOption("s", "Migration tree file", migrationTreeFile);
  ap.other("frequencies", "Frequencies");
  ap.parse();
  
  if (ap.files().size() != 1)
  {
    std::cerr << "Error: <frequencies> must be specified" << std::endl;
    return 1;
  }
  
  std::string filenameFrequencies = ap.files()[0];
  
  StringToIntMap colorMap;
  if (!filenameColorMap.empty())
  {
    std::ifstream inColorMap(filenameColorMap.c_str());
    if (!inColorMap.good())
    {
      std::cerr << "Could not open '" << filenameColorMap << "' for reading" << std::endl;
      return 1;
    }
    
    if (!BaseTree::readColorMap(inColorMap, colorMap))
    {
      return 1;
    }
  }

  FrequencyMatrix F;
  try
  {
    if (filenameFrequencies != "-")
    {
      std::ifstream inFrequencies(filenameFrequencies.c_str());
      if (!inFrequencies.good())
      {
        std::cerr << "Could not open '" << filenameFrequencies << "' for reading" << std::endl;
        return 1;
      }
      inFrequencies >> F;
      inFrequencies.close();
    }
    else
    {
      std::cin >> F;
    }
  }
  catch (std::runtime_error& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  
  int primaryIndex = F.anatomicalSiteToIndex(primary);
  if (primaryIndex == -1)
  {
    std::cerr << "Error: primary '" << primary << "' is undefined" << std::endl;
    return 1;
  }
  
  if (F.mS(primaryIndex))
  {
    std::cerr << "Provided frequency matrix is potentially generated by a clone tree with a monoclonal single-source seeding pattern" << std::endl;

    MSEnumeration enumerate(F, primary, outputDirectory, colorMap);

    // MEK: disabled for now, refinement code is buggy.
    // And algorithm needs some theoretical some work
//    if (migrationTreeFile.empty())
//    {
//      enumerate.run(true, limit);
//    }
//    else
//    {
//      MigrationTree migrationTree;
//      std::ifstream inFile(migrationTreeFile.c_str());
//      if (!inFile.good())
//      {
//        std::cerr << "Could not open '" << migrationTreeFile << "' for reading" << std::endl;
//        return 1;
//      }
//      migrationTree.readWithPrimary(inFile, primary);
////      std::cout << migrationTree << std::endl;
//
//      enumerate.run(migrationTree, true, "", limit);
//    }
//
//    int nrSolutions = enumerate.getNrSolutions();
//    std::cout << "Found " << nrSolutions << " solutions." << std::endl;
//
//    for (int idx = 0; idx < nrSolutions; ++idx)
//    {
//      char buf[1024];
//      snprintf(buf, 1024, "T%d.dot", idx);
//      std::ofstream outFile(buf);
//      enumerate.getSolution(idx).writeDOT(outFile, colorMap);
//      outFile.close();
//    }
  }
  else
  {
    std::cerr << "Provided frequency matrix is NOT generated by a clone tree with a monoclonal single-source seeding pattern" << std::endl;
  }
  
  return 0;
}
