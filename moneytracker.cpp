/*
  Copyright (c) 2014 Auston Sterling
  See LICENSE for copying permissions.
  
  -----House Money Tracker-----
  Auston Sterling
  austonst@gmail.com

  Parses a file or reads from command line to get info, then computes and displays
  how much each person owes each other person.
*/

#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <vector>
#include <cstdlib>
#include "group.h"
#include "person.h"

const int MAX_INPUT_LENGTH = 200;

//List iterators
typedef std::list<Person>::iterator personIt;
typedef std::list<Group>::iterator groupIt;

//Takes the list of all persons and a person and returns a number saying how much
//total debt everyone owes that person.
int debtTo(std::list<Person>& persons, std::string p)
{
  int sum = 0;
  for (std::list<Person>::iterator i = persons.begin(); i != persons.end(); i++)
    {
      sum += i->debt(p);
    }

  return sum;
}

//Takes a given input stream and parses it, modifying the provided lists
//Reads until EOF is found. This will assume an "All" group already exists.
//Returns 0 if it succeeded, returns 1 otherwise.
int parseInput(std::istream & input, std::list<Group> & groups, std::list<Person> & persons)
{
  //Set up some variables
  int lineNum = 0;

  //Get an iterator to group All
  groupIt all;
  for (all = groups.begin(); all != groups.end(); all++)
    {
      if (all->name() == "All") break;
    }
  
  //Read until EOF
  while (!input.eof())
    {
      //If reading from terminal, place a prompt
      if (input == std::cin) std::cout << "> ";

      //Increment lineNum
      lineNum++;
      
      //Get lines one at a time
      char buf[MAX_INPUT_LENGTH];
      input.getline(buf, MAX_INPUT_LENGTH);
      std::stringstream lineInput(buf);

      //Now parse the individual line
      std::string item;
      std::vector<std::string> tokens;
      while (std::getline(lineInput, item, ' '))
	{
	  tokens.push_back(item);
	}

      //Erase all empty lines and windows line returns
      for (std::vector<std::string>::iterator i = tokens.begin(); i != tokens.end();)
	{
	  if ((*i).size() == 0 || (*i) == "\r")
	    {
	      i = tokens.erase(i);
	    }
	  else
	    {
	      i++;
	    }
	}

      //Empty line
      if (tokens.size() == 0) continue;

      //Remove trailing \r
      if (tokens[tokens.size()-1].find('\r') != std::string::npos)
        {
          tokens[tokens.size()-1].erase(tokens[tokens.size()-1].find('\r'), 1);
        }

      //Comments are done by starting the line with a %
      if (tokens[0][0] == '%')
	{
	  //Do nothing
	}

      //group command: Create new group
      //group GROUPNAME
      else if (tokens[0] == "group")
	{
	  //Verify line length
	  if (tokens.size() != 2)
	    {
	      std::cerr << "ERROR: Group command only takes one argument.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }

	  //Verify this group name is not taken
	  bool taken = false;
	  for (groupIt i = groups.begin(); i != groups.end(); i++)
	    {
	      if (i->name() == tokens[1])
		{
		  std::cerr << "WARNING: Group name \"" << tokens[1] << "\" already in use.\n" <<
		    "Warning occurred at line " << lineNum << ".\n";
		  taken = true;
		}
	    }

	  //Add the group!
	  if (!taken) groups.push_back(Group(tokens[1]));
	}

      //person command: Create new person
      //person PERSONNAME
      else if (tokens[0] == "person")
	{
	  //Verify line length
	  if (tokens.size() != 2)
	    {
	      std::cerr << "ERROR: Person command only takes one argument.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }

	  //Verify this person name is not taken
	  bool taken = false;
	  if (all->hasMember(tokens[1]))
	    {
	      std::cerr << "WARNING: Person name \"" << tokens[1] << "\" already in use.\n" <<
		"Warning occurred at line " << lineNum << ".\n";
	      taken = true;
	    }
	  
	  //Add the person and place them in the "All" group
	  if (!taken) all->addPerson(tokens[1]);
	  persons.push_back(Person(tokens[1]));
	}

      //join command: Add people to a group
      //join GROUPNAME PERSONNAME1 PERSONNAME2 ...
      else if (tokens[0] == "join")
	{
	  //Verify line length
	  if (tokens.size() < 3)
	    {
	      std::cerr << "ERROR: Join command must have at least two arguments.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }

	  //Ensure the group exists
	  groupIt g;
	  for (g = groups.begin(); g != groups.end(); g++)
	    {
	      if (g->name() == tokens[1]) break;
	    }
	  if (g == groups.end())
	    {
	      std::cerr << "ERROR: Group " << tokens[1] << " does not exist.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }
	  
	  //Add each person, ensuring no duplicates
	  for (size_t i = 2; i < tokens.size(); i++)
	    {
	      //Ensure this person exists by querying the "All" group
	      if (!all->hasMember(tokens[i]))
		{
		  std::cerr << "ERROR: Person " << tokens[i] << " does not exist.\n" <<
		    "Stopped parsing at line " << lineNum << ".\n";
		  return 1;
		}
	      
	      g->addPerson(tokens[i]);
	    }
	}

      //leave command: Remove people from group
      //leave GROUPNAME PERSONNAME1 PERSONNAME2 ...
      else if (tokens[0] == "leave")
	{
	  //Verify line length
	  if (tokens.size() < 3)
	    {
	      std::cerr << "ERROR: Leave command must have at least two arguments.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }

	  //Ensure the group exists
	  groupIt g;
	  for (g = groups.begin(); g != groups.end(); g++)
	    {
	      if (g->name() == tokens[1]) break;
	    }
	  if (g == groups.end())
	    {
	      std::cerr << "ERROR: Group " << tokens[1] << " does not exist.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }

	  //Remove each person!
	  for (size_t i = 2; i < tokens.size(); i++)
	    {
	      //If the person was not in the group, error
	      if (!g->removePerson(tokens[i]))
		{
		  std::cerr << "ERROR: Person " <<
		    tokens[i] << " is not in group " << tokens[1] << ".\n" <<
		    "Stopped parsing at line " << lineNum << ".\n";
		  return 1;
		}
	    }
	}

      //groupdel command: Delete a group
      //groupdel GROUPNAME
      else if (tokens[0] == "groupdel")
	{
	  //Verify line length
	  if (tokens.size() != 2)
	    {
	      std::cerr << "ERROR: Groupdel command only takes one argument.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }

	  //Ensure this group exists
	  groupIt g;
	  for (g = groups.begin(); g != groups.end(); g++)
	    {
	      if (g->name() == tokens[1]) break;
	    }
	  if (g == groups.end())
	    {
	      std::cerr << "ERROR: Group " << tokens[1]<<" already does not exist.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }

	  //Delete it
	  groups.erase(g);
	}
      
      //persondel command: Delete a person, also removing their debt
      //persondel PERSONNAME
      else if (tokens[0] == "persondel")
	{
	  //Verify line length
	  if (tokens.size() != 2)
	    {
	      std::cerr << "ERROR: Persondel command only takes one argument.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }
	  
	  //Ensure this person exists
	  if (!all->hasMember(tokens[1]))
	    {
	      std::cerr << "ERROR: Person " << tokens[1] << " already does not exist.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }
	  
	  //Remove this person from all groups
	  for (groupIt g = groups.begin(); g != groups.end(); g++)
	    {
	      g->removePerson(tokens[1]);
	    }

	  personIt p;
	  for (p = persons.begin(); p != persons.end(); p++)
	    {
	      if (p->name() == tokens[1]) break;
	    }
	  persons.erase(p);
	}
      
      //tx command: Record a transaction between persons
      //tx PAYER AMOUNT CATEGORY PERSONNAME1 group GROUPNAME1 PERSONNAME2
      else if (tokens[0] == "tx")
	{
	  //Verify line length
	  if (tokens.size() < 5)
	    {
	      std::cerr << "ERROR: tx command takes at least 5 arguments.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }

	  //Ensure payer exists
	  if (!all->hasMember(tokens[1]))
	    {
	      std::cerr << "ERROR: Person " << tokens[1] << " does not exist.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }

	  //Ensure the amount is a number
	  int amount = (atof(tokens[2].c_str()) * 100.0) + 0.5;
	  if (amount == 0)
	    {
	      std::cerr << "ERROR: Amount must be a number greater than 0.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }

	  //Find the amount each person will spend
	  std::set<std::string> spenders;
	  
	  //Go through every other person
	  //Do a verification and counting run
	  for (size_t i = 4; i < tokens.size(); i++)
	    {
	      //If it is a group
	      if (tokens[i] == "group")
		{
		  //Ensure the group exists
		  i++;
		  if (i == tokens.size())
		    {
		      std::cerr << "ERROR: No group specified.\n" <<
			"Stopped parsing at line " << lineNum << ".\n";
		      return 1;
		    }
		  
		  //Ensure the group exists
		  groupIt g;
		  for (g = groups.begin(); g != groups.end(); g++)
		    {
		      if (g->name() == tokens[i]) break;
		    }
		  
		  if (g == groups.end())
		    {
		      std::cerr << "ERROR: Group " << tokens[i] << " does not exist.\n" <<
			"Stopped parsing at line " << lineNum << ".\n";
		      return 1;
		    }
		  
		  //Add these people to the set
		  spenders.insert(g->persons().begin(), g->persons().end());
		  continue;
		}
	      
	      //Ensure they exist
	      if (!all->hasMember(tokens[i]))
		{
		  std::cerr << "ERROR: Person " << tokens[i] << " does not exist.\n" <<
		    "Stopped parsing at line" << lineNum << ".\n";
		  return 1;
		}
	      
	      //Add this person to the set
	      spenders.insert(tokens[i]);
	    }
	  
	  //See how much each person pays
	  //If the person who spent the money is also paying
	  unsigned int numSpenders;
	  if (spenders.find(tokens[1]) != spenders.end())
	    {
	      numSpenders = spenders.size();
	    }
	  else
	    {
	      numSpenders = spenders.size();
	    }
	  
	  int perPerson = amount/numSpenders;
	  
	  //Go over each person, add this debt
	  for (std::set<std::string>::iterator i = spenders.begin(); i != spenders.end(); i++)
	    {
	      //Check for payer
	      if ((*i) == tokens[1])
		{
		  if (numSpenders != spenders.size()) continue;
		}
	      
	      //Add it!
	      personIt p;
	      for (p = persons.begin(); p != persons.end(); p++)
		{
		  if (p->name() == (*i)) break;
		}
	      p->addDebt(tokens[1], perPerson, tokens[3]);
	    }
	}

      //debt command: Display how much one person owes another person (or overall)
      //debt PERSONNAME1 [PERSONNAME2]
      else if (tokens[0] == "debt")
	{
	  //Verify input length
	  if (tokens.size() > 3)
	    {
	      std::cerr << "ERROR: debt command takes no more than 2 arguments.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }
	  else if (tokens.size() == 3)
	    {
	      //Ensure both people exist
	      if (!all->hasMember(tokens[1]))
		{
		  std::cerr << "ERROR: person " << tokens[1] << " does not exist.\n" <<
		    "Stopped parsing at line " << lineNum << ".\n";
		  return 1;
		}
	      if (!all->hasMember(tokens[2]))
		{
		  std::cerr << "ERROR: person " << tokens[2] << " does not exist.\n" <<
		    "Stopped parsing at line " << lineNum << ".\n";
		  return 1;
		}

	      //Print the debt 1 owes 2 minus the debt 2 owes 1
	      personIt p1, p2;
	      for (p1 = persons.begin(); p1 != persons.end(); p1++)
		{
		  if (tokens[1] == p1->name()) break;
		}
	      for (p2 = persons.begin(); p2 != persons.end(); p2++)
		{
		  if (tokens[2] == p2->name()) break;
		}

	      float total = float(p1->debt(p2->name()))/100.0 - float(p2->debt(p1->name()))/100.0;

	      std::cout << tokens[1] << " owes " << tokens[2] << " $" << total << ".\n";
	    }
	  else if (tokens.size() == 2)
	    {
	      //Ensure the person exists
	      if (!all->hasMember(tokens[1]))
		{
		  std::cerr << "ERROR: person " << tokens[1] << " does not exist.\n" <<
		    "Stopped parsing at line " << lineNum << ".\n";
		  return 1;
		}

	      //Print the total debt 1 owes
	      personIt p1;
	      for (p1 = persons.begin(); p1 != persons.end(); p1++)
		{
		  if (tokens[1] == p1->name()) break;
		}

	      float total = float(p1->totalDebt())/100.0 - float(debtTo(persons, tokens[1]))/100.0;

	      std::cout << tokens[1] << " owes $" << total << " total." << std::endl;
	    }
	  else
	    {
	      std::cerr << "ERROR: debt command requires at least one argument.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }
	}

      //load command: loads from a file
      //load FILENAME
      else if (tokens[0] == "load")
        {
          //Verify input length
	  if (tokens.size() != 2)
	    {
	      std::cerr << "ERROR: load command takes only one argument.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }
          
          //Make sure this is a file we can use
          std::ifstream fin(tokens[1].c_str(), std::ifstream::in);
          if (!fin)
            {
              std::cerr << "ERROR: Could not find/open file " << tokens[1] <<
                "\nStopped parsing at line " << lineNum << ".\n";
              return 1;
            }

          //Read it in
          if (parseInput(fin, groups, persons) != 0)
            {
              std::cerr << "ERROR: Failed to read file " << tokens[1] <<
                "\nStopped parsing at line " << lineNum << ".\n";
              fin.close();
              return 1;
            }

          //Close it up
          fin.close();

          //Notify user
          std::cout << "Read input from " << tokens[1] << ".\n";
        }

      //quit command: exit the program
      //quit
      else if (tokens[0] == "quit")
	{
	  std::cout << "Bye!\n";
	  return 0;
	}

      //info command: Display info about one specific group or person
      //info PERSONNAME | group GROUPNAME
      else if (tokens[0] == "info")
	{
	  //Verify input length
	  if (tokens.size() > 3 || tokens.size() == 1)
	    {
	      std::cerr << "ERROR: info command takes no more than 2 arguments.\n" <<
		"Stopped parsing at line " << lineNum << ".\n";
	      return 1;
	    }

	  //If it's requesting person info
	  else if (tokens.size() == 2)
	    {
	      //The person must exist
	      if (!all->hasMember(tokens[1]))
		{
		  std::cerr << "ERROR: Person " << tokens[1] << " does not exist.\n" <<
		    "Stopped parsing at line " << lineNum << ".\n";
		  return 1;
		}

	      //Find him/her
	      personIt p;
	      for (p = persons.begin(); p != persons.end(); p++)
		{
		  if (p->name() == tokens[1]) break;
		}

	      //Print out info
	      std::cout << "-----Info for " << tokens[1] << "-----\n";
	      std::cout << "Total debt: $" <<
		float(p->totalDebt())/100.0 - float(debtTo(persons, tokens[1]))/100.0 << ".\n\n";
	      
	      for (std::set<std::string>::const_iterator g = all->persons().begin(); g != all->persons().end(); g++)
		{
		  //Skip this person
		  if (p->name() == (*g)) continue;

		  personIt p2;
		  for (p2 = persons.begin(); p2 != persons.end(); p2++)
		    {
		      if (p2->name() == (*g)) break;
		    }

		  float total = float(p->debt(p2->name()))/100.0 - float(p2->debt(p->name()))/100.0;
		  
		  std::cout << tokens[1] << " owes " << (*g) << " $" << total << ".\n";
		  for (std::list<std::pair<std::string,int> >::const_iterator i = p->history((*g)).begin(); i != p->history((*g)).end(); i++)
		    {
		      float theTotal = float(i->second)/100.0;
		      std::cout << "  " << i->first << ": " << theTotal << ".\n";
		    }
                  for (std::list<std::pair<std::string,int> >::const_iterator i = p2->history((tokens[1])).begin(); i != p2->history((tokens[1])).end(); i++)
		    {
		      float theTotal = float(i->second)/100.0;
		      std::cout << "  " << i->first << ": -" << theTotal << ".\n";
		    }

		  std::cout << std::endl;
		}
	    }
	  else //tokens.size() == 3
	    {
	      //FILL IN HERE
	    }
	}

      //Help command
      else if(tokens[0] == "help")
        {
          //If no arguments
          if (tokens.size() == 1)
            {
              std::cout << "Available commands: person group join leave groupdel persondel tx debt info load quit\n";
            }
          else //Two or more arguments
            {
              if (tokens[1] == "person")
                {
                  std::cout << "Adds a person.\nperson PERSONNAME\n";
                }
              else if (tokens[1] == "group")
                {
                  std::cout << "Adds a group.\ngroup GROUPNAME\n";
                }
              else if (tokens[1] == "join")
                {
                  std::cout << "Adds a person to a group.\njoin GROUPNAME PERSONNAME1 [PERSONNAME2 ...]\n";
                }
              else if (tokens[1] == "leave")
                {
                  std::cout << "Removes a person from a group.\nleave GROUPNAME PERSONNAME1 [PERSONNAME2 ...]\n";
                }
              else if (tokens[1] == "groupdel")
                {
                  std::cout << "Removes a group.\ngroupdel GROUPNAME\n";
                }
              else if (tokens[1] == "persondel")
                {
                  std::cout << "Removes a person and erases all their debt.\npersondel PERSONNAME\n";
                }
              else if (tokens[1] == "tx")
                {
                  std::cout << "Creates a transaction.\ntx PERSONNAME AMOUNT DESCRIPTION PERSONNAME1 [PERSONNAME2 ...]\n";
                }
              else if (tokens[1] == "debt")
                {
                  std::cout << "Displays a person's debt overall, or just to one person.\ndebt PERSONNAME1 [PERSONNAME2]\n";
                }
              else if (tokens[1] == "info")
                {
                  std::cout << "Displays information about a person.\ninfo PERSONNAME\n";
                }
              else if (tokens[1] == "load")
                {
                  std::cout << "Loads from a file.\nload FILENAME\n";
                }
              else if (tokens[1] == "quit")
                {
                  std::cout << "Exits the program.\n";
                }
              else if (tokens[1] == "help")
                {
                  std::cout << "Prints *this.\n";
                }
              else
                {
                  std::cerr << "WARNING: Command \"" << tokens[1] << "\" does not exist.\n" <<
                    "Warning occurred at line " << lineNum << ".\n";
                }
            }
        }

      //Unrecognized command
      else
	{
	  std::cerr << "Unrecognized command " << tokens[0] << ".\n" <<
	    "Stopped parsing at line " << lineNum << ".\n";
	  return 1;
	}
	      
    }

  return 0;
}

//Main function
int main(int argc, char* argv[])
{
  //Check input
  if (argc > 2)
    {
      std::cerr << "Usage: " << argv[0] << " [Transaction File]\n";
      return 1;
    }

  //Set up initial structures
  //Set up one initial group for all Persons
  std::list<Group> groups;
  groups.push_back(Group("All"));
  std::list<Person> persons;

  //Check for input file
  if (argc == 2)
    {
      //Make sure this is a file we can use
      std::ifstream fin(argv[1], std::ifstream::in);
      if (!fin)
	{
	  std::cerr << "Could not find/open file " << argv[1] << "\n";
	  return 1;
	}

      //Read it in
      parseInput(fin, groups, persons);

      //Close it up
      fin.close();

      //Notify user
      std::cout << "Read input from " << argv[1] << ".\n";
    }

  //Pass input off to stdin
  std::cout << "House Money Tracker\n" <<
    "Type \"quit\" to end the program." << std::endl;
  int ret = 1;
  while (ret != 0) ret = parseInput(std::cin, groups, persons);
}
