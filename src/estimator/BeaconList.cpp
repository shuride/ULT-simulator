#include <stdio.h>
#include <stdlib.h>
#include "BeaconList.h"

#define MIN_SEP 10


BeaconList::BeaconList()
{
	baseBeaconList.clear();
}

BeaconList::~BeaconList()
{
	reset();
}

void BeaconList::reset()
{
	for (size_t i = 0; i < baseBeaconList.size(); i++)
		delete baseBeaconList[i];
	baseBeaconList.clear();
	
}

void BeaconList::addBeacon(int userBid, Vector location)
{
	int bid = baseBeaconList.size();
	Beacon *beacon = new Beacon(bid, location);
	beacon->setUserBid(userBid);
	addBeacon(beacon);
}

Beacon* BeaconList::findByUserBid(int userBid)
{
	for (size_t i = 0; i < baseBeaconList.size(); i++)
	{
		if (baseBeaconList[i]->getUserBid() == userBid)
			return baseBeaconList[i];
	}
	return NULL;
}

void BeaconList::addBeacon(Beacon *beacon)
{
	baseBeaconList.push_back(beacon);
}

void BeaconList::applyPlanes(PlaneList *planeList, int maxReflectionCount)
{
	this->planeList = planeList;
	this->maxReflectionCount = maxReflectionCount;

	for (size_t i = 0; i < baseBeaconList.size(); i++)
	{
		applyPlanes(baseBeaconList[i]);

		// after planes are added, make iterators
		baseBeaconList[i]->setIterator(false/*isDFS*/);
	}

	
	
}

void BeaconList::applyPlanes(Beacon *beacon)
{
	if (beacon->getReflectionCount() == maxReflectionCount) return;

	for (size_t i = 0; i < planeList->size(); i++)
	{
		int pid = beacon->getLastPlaneId();

		// signal can not be reflected on same plane twice
		if (pid >= 0 && pid == planeList->at(i)->getPid()) 
			continue;

		// if beacon is too close to plane, do not reflect
		if (planeList->at(i)->getDistanceToPoint(beacon->getLocation()) < MIN_SEP)
			continue;

		beacon->addReflectedBeacon(planeList->at(i));
	}

	for (size_t i = 0; i < beacon->childrenSize(); i++)
	{
		applyPlanes(beacon->childAt(i));
	}
}


/*
void BeaconList::makeIterators()
{
	for (size_t i = 0; i < baseBeaconList.size(); i++)
	{
		BeaconIterator *iter = baseBeaconList[i]->getIterator();

		iter->makeIndex(baseBeaconList[i], false);

		beaconIterators.push_back(iter);
		baseBeaconList[i]->setIterator(&beaconIterators[i]);
	}
}

void BeaconList::setup()
{
	beacons = baseBeaconList;
	for (size_t i = 0; i < beaconIterators.size(); i++)
	{
		beacons[i] = beaconIterators.reset();
	}
}


bool BeaconList::moveNext(int bid)
{
}
*/

size_t BeaconList::size()
{
	return baseBeaconList.size();
}

Beacon* BeaconList::at(int idx)
{
	return baseBeaconList[idx];
}

void BeaconList::load(const char* filename)
{
	reset();

	FILE *fp = fopen(filename, "r");

	if (fp == NULL)
	{
		printf("can't open beacon list file %s\n", filename);
		exit(19);
	}

	const int bufSize = 1024;
	char buf[bufSize + 1];


	while(fgets(buf, bufSize, fp) != NULL)
	{
		int bid;
		Vector location;

		bid = atoi(strtok(buf, ";"));

		location.x = atof(strtok(NULL, ";"));
		location.y = atof(strtok(NULL, ";"));
		location.z = atof(strtok(NULL, ";\r\n#"));

		addBeacon(bid, location);
	}
	fclose(fp);
}


void BeaconList::save(const char* filename)
{
	FILE *fp = fopen(filename , "w");

	if (fp == NULL)
	{
		printf("BeaconList::save, can't open file %s\n", filename);
		exit(20);
	}

	for (size_t i = 0; i < baseBeaconList.size(); i++)
	{
		Vector location = baseBeaconList[i]->getLocation();
		fprintf(fp, "%2d;%5d;%5d;%5d;\n", 
				baseBeaconList[i]->getUserBid(),
				(int)location.x, (int)location.y, (int)location.z);
	}
	fclose(fp);
}

// this function does not sustain beacon structure.
// after this function reset beaconlist and re-apply planes should be done
void BeaconList::modifyAndSave(const char *filename, int userBid, Vector modifiedLocation)
{
	FILE *fp = fopen(filename , "w");

	if (fp == NULL)
	{
		printf("BeaconList::save, can't open file %s\n", filename);
		exit(20);
	}

	for (size_t i = 0; i < baseBeaconList.size(); i++)
	{
		Vector location = baseBeaconList[i]->getLocation();
		
		if (baseBeaconList[i]->getUserBid() == userBid)
			location = modifiedLocation;
		
		fprintf(fp, "%2d;%5d;%5d;%5d;\n", 
				baseBeaconList[i]->getUserBid(),
				(int)location.x, (int)location.y, (int)location.z);

	}
	fclose(fp);
}
// this function does not sustain beacon structure.
// after this function reset beaconlist and re-apply planes should be done
void BeaconList::deleteAndSave(const char *filename, int userBid)
{
	FILE *fp = fopen(filename , "w");

	if (fp == NULL)
	{
		printf("BeaconList::save, can't open file %s\n", filename);
		exit(20);
	}

	for (size_t i = 0; i < baseBeaconList.size(); i++)
	{
		Vector location = baseBeaconList[i]->getLocation();
		
		if (baseBeaconList[i]->getUserBid() == userBid)
			continue;
		
		fprintf(fp, "%2d;%5d;%5d;%5d;\n", 
				baseBeaconList[i]->getUserBid(),
				(int)location.x, (int)location.y, (int)location.z);

	}
	fclose(fp);
}