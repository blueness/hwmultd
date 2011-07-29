/*
	This file is part of hwmultd.
	Copyright (C) 2011  Anthony G. Basile

	hwmultd is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	hwmultd is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <clcommon.h>


//Every function does nothing successfully

int
init_cl()
{
	return CL_SUCCESS;
}

int
reset_cl()
{
	return CL_SUCCESS;
}

char *
act_cl(char *msg)
{
	return "";
}

int
close_cl()
{
	return CL_SUCCESS;
}
