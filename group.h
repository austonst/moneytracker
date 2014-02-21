/*
  Copyright (c) 2014 Auston Sterling
  See LICENSE for copying permissions.
  
  -----Group Class Header File-----
  Auston Sterling
  austonst@gmail.com

  Contains the header for a class grouping Persons for easy use.
*/

#ifndef _group_h_
#define _group_h_

#include <set>
#include <string>

class Group
{
 public:
  //Constructors
  Group(std::string inname);

  //Accessors
  const std::string & name() const {return name_;}
  const std::set<std::string> & persons() const {return persons_;}
  int size() const {return persons_.size();}

  //Mutators
  void setName(const std::string & inname) {name_ = inname;}
  bool addPerson(const std::string & inname) {return persons_.insert(inname).second;}
  bool removePerson(const std::string & inname) {return persons_.erase(inname) > 0;}

  //General use functions
  bool hasMember(const std::string & inname) const {return persons_.find(inname) != persons_.end();}

 private:
  //The name of the group
  std::string name_;

  //The list of Person names in the group
  std::set<std::string> persons_;
};

#endif
