/*
  Copyright (c) 2014 Auston Sterling
  See LICENSE for copying permissions.
  
  -----Person Header File-----
  Auston Sterling
  austonst@gmail.com

  Contains the header for a class detailing a person and their debts.
*/

#ifndef _person_h_
#define _person_h_

#include <string>
#include <map>
#include <list>

class Person
{
 public:
  //Constructors
  Person(std::string inname);

  //Accessors
  const std::string & name() const {return name_;}
  int debt(std::string payer);
  const std::list<std::pair<std::string,int> > & history(std::string payer) {return debt_[payer];}

  //Mutators
  void setName(std::string inname) {name_ = inname;}

  //General use functions
  void addDebt(std::string payer, int amount, std::string desc);
  int totalDebt();

 private:
  //The person's name
  std::string name_;

  //A map of other people's names, and the unresolved tx history with them
  std::map<std::string, std::list<std::pair<std::string,int> > > debt_;
};

#endif
