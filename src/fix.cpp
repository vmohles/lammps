/* ----------------------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   http://lammps.sandia.gov, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under 
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#include "string.h"
#include "ctype.h"
#include "fix.h"
#include "group.h"
#include "error.h"

using namespace LAMMPS_NS;

/* ---------------------------------------------------------------------- */

Fix::Fix(LAMMPS *lmp, int narg, char **arg) : Pointers(lmp)
{
  // fix ID, group, and style
  // ID must be all alphanumeric chars or underscores

  int n = strlen(arg[0]) + 1;
  id = new char[n];
  strcpy(id,arg[0]);

  for (int i = 0; i < n-1; i++)
    if (!isalnum(id[i]) && id[i] != '_')
      error->all("Fix ID must be alphanumeric or underscore characters");

  igroup = group->find(arg[1]);
  if (igroup == -1) error->all("Could not find fix group ID");
  groupbit = group->bitmask[igroup];

  n = strlen(arg[2]) + 1;
  style = new char[n];
  strcpy(style,arg[2]);

  restart_global = 0;
  restart_peratom = 0;
  force_reneighbor = 0;
  box_change = 0;
  thermo_energy = 0;
  rigid_flag = 0;
  virial_flag = 0;
  no_change_box = 0;

  scalar_flag = vector_flag = peratom_flag = 0;

  comm_forward = comm_reverse = 0;

  // mask settings - same as in modify.cpp

  INITIAL_INTEGRATE = 1;
  PRE_EXCHANGE = 2;
  PRE_NEIGHBOR = 4;
  POST_FORCE = 8;
  FINAL_INTEGRATE = 16;
  END_OF_STEP = 32;
  THERMO_ENERGY = 64;
  INITIAL_INTEGRATE_RESPA = 128;
  POST_FORCE_RESPA = 256;
  FINAL_INTEGRATE_RESPA = 512;
  MIN_POST_FORCE = 1024;
  MIN_ENERGY = 2048;
}

/* ---------------------------------------------------------------------- */

Fix::~Fix()
{
  delete [] id;
  delete [] style;
}

/* ----------------------------------------------------------------------
   process params common to all fixes here
   if unknown param, call modify_param specific to the fix
------------------------------------------------------------------------- */

void Fix::modify_params(int narg, char **arg)
{
  if (narg == 0) error->all("Illegal fix_modify command");

  int iarg = 0;
  while (iarg < narg) {
    if (strcmp(arg[iarg],"energy") == 0) {
      if (iarg+2 > narg) error->all("Illegal fix_modify command");
      if (strcmp(arg[iarg+1],"no") == 0) thermo_energy = 0;
      else if (strcmp(arg[iarg+1],"yes") == 0) thermo_energy = 1;
      else error->all("Illegal fix_modify command");
      iarg += 2;
    } else {
      int n = modify_param(narg-iarg,&arg[iarg]);
      if (n == 0) error->all("Illegal fix_modify command");
      iarg += n;
    }
  }
}
