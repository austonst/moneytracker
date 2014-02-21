/*
  Copyright (c) 2014 Auston Sterling
  See LICENSE for copying permissions.
  
  -----Person Implementation File-----
  Auston Sterling
  austonst@gmail.com

  Contains the implementation for a class detailing a person and their debts.
*/

#include "person.h"

//Standard use constructor
Person::Person(std::string inname) : name_(inname) {}

//Returns the total debt this person owes to another person
int Person::debt(std::string payer)
{
  //Iterate over all previous payments, sum them up
  int sum = 0;
  for (std::list<std::pair<std::string,int> >::iterator i = debt_[payer].begin(); i != debt_[payer].end(); i++)
    {
      sum += i->second;
    }

  return sum;
}

//Adds some debt this person must pay
void Person::addDebt(std::string payer, int amount, std::string desc)
{
  debt_[payer].push_back(make_pair(desc, amount));
}

//Returns the sum of this person's debt to everyone
int Person::totalDebt()
{
  //Iterate over EVERYTHING, sum up
  int sum = 0;
  for (std::map<std::string, std::list<std::pair<std::string,int> > >::iterator i = debt_.begin(); i != debt_.end(); i++)
    {
      for (std::list<std::pair<std::string,int> >::iterator j = i->second.begin(); j != i->second.end(); j++)
	{
	  sum += j->second;
	}
    }

  return sum;
}
