#include <limits>
#include <cfloat>     // FLT_MAX
#include <string.h>
#include <bits/stdc++.h>
#include <yaml.h>
#include <algorithm>
#include "BridgeCrossing.h"

// Globals Data
static const int HikerMoveTimeSize = (1 << 20);
static float **HikerMoveTime = nullptr;

static vector <HikerInfo> GetHikerInfoForBridge(const BridgeInfo & bridge);
static int GetSetBitCounts(int Num);
static void PrintBinaryBits(int Num);
static bool HasMinTimeComputed(float mintime);

static int GetSetBitCounts(int Num)
{
	int count = 0;

        while (Num > 0)
        {
                count += (Num & 1);

                Num = Num >> 1;
        }

        return count;
}

static void PrintBinaryBits(int Num)
{
	// This was implented to debug code during implemention phase
	unsigned i;
	cout << "Binary number for Num: "<< Num << ", is=";
        for (i = 1 << 31; i > 0; i = i / 2)
	{
        	(Num & i)? cout<<"1": cout <<"0";
	}

	cout << endl;
}

void AllocateMinTimeArray()
{
	if (HikerMoveTime)
	{
		// Alreday initialized
		return;
	}
	HikerMoveTime = new float *[HikerMoveTimeSize];
	for (auto i = 0; (i < HikerMoveTimeSize); ++i)
	{
		HikerMoveTime[i] = new float [2];
		memset(HikerMoveTime[i], -1, sizeof(float) * 2);
	}	
}

void DeallocateMinTimeArray()
{
	if (!HikerMoveTime)
	{
		return;
	}
	for (auto i = 0; (i < HikerMoveTimeSize); ++i)
	{
		delete [] HikerMoveTime[i];
	}

	delete [] HikerMoveTime;
	HikerMoveTime = nullptr;
}

void InitMinTimeArray()
{
	if (!HikerMoveTime)
	{
		cout << "Mintime array is not initialized, initializing ..."<< endl;
		AllocateMinTimeArray();
		return;
	}
	for (auto i = 0; (i < HikerMoveTimeSize); ++i)
        {
                memset(HikerMoveTime[i], -1, sizeof(float) * 2);
        }
}

void BridgeAndHikersSolution::ComputeTimeToCrossCurrentBridge(const BridgeInfo & bridge,
                                                              const vector<HikerInfo> &hikers)
{
	BridgeAndHikersInfo bhinfo(bridge, hikers);
	if (AllBridgeAndHikersList.size() > 0)
	{
		// Populate Hikers from previous Bridges which is in the last entry of vector
		vector<HikerInfo> &PrevHikers = AllBridgeAndHikersList.at(AllBridgeAndHikersList.size() - 1).GetHikers();
		for (size_t i = 0; (i < PrevHikers.size()); ++i)
		{
			/*
			 * Old hikers will join with the new hikers to cross the new bridge
			 */
			HikerInfo Hiker = PrevHikers.at(i);
			bhinfo.GetHikers().push_back(Hiker);
		}
	}

	bhinfo.ComputeBridgeCrossTime();

	AllBridgeAndHikersList.push_back(bhinfo);
}

static bool HasMinTimeComputed(float mintime)
{
	unsigned int a;

	memcpy(&a, &mintime, sizeof(a));

	if (~a)
	{
		return true;
	}

	return false;
}

void BridgeAndHikersInfo::ComputeBridgeCrossTime()
{
	/*
         * Compute: How much time an old hiker need to cross oncei, this new
         * Bridge whose length may be different.
         */

        InitHikerInfoForNewBridge();
        int NumHiker = HikersList.size();

	if (NumHiker == 0)
	{
		cerr << "Error: There is no Hiker to cross the bridge, id=" << aBridge.GetBridgeId() << endl;
		return;
	}

        /*
         * Init:
         *  All bits of HikersWhoDidntCross will be 1
         *  It means no hikers corssed Bridge yet.
         *
         *  If Bit# 1 of HikersCrossInfo set to 1, hiker#1 has not crossed the bridge
         *  If Bit# 1 HikersCrossInfo set to 0, hiker#1 has crossed the bridge
         */
        int HikersCrossInfo = (1 << NumHiker) - 1;

        MinTimeToCross = ComputeHikersTimeToCrossBridge (HikersCrossInfo, CROSS_BRIDGE);
}

float BridgeAndHikersInfo::ComputeHikersTimeToCrossBridge(int HikersCrossingInfo, bool moveDirection)
{
	int NumHiker = HikersList.size();

	if (!HikersCrossingInfo)
	{
		// All hikers crossed
		return 0;
	}

	float & MinTime = HikerMoveTime[HikersCrossingInfo][moveDirection];

	if (HasMinTimeComputed(MinTime))
	{
		/*
		 * Solved Partially
		 */
		return MinTime;
	}


	if (moveDirection == UNDO_CROSSING)
	{
		/*
		 * A Hiker who crossed will come back with torch to help cross other hikers
		 */

		float MinTimeOfCrossedHiker = FLT_MAX;
		int HikerId;
		bool found = false;

		int HikersWhoCrossedBridge = ((1 << NumHiker) - 1) ^ HikersCrossingInfo;

		for (int i = 0; (i < NumHiker); ++i)
		{
			/*
			 * Find a hiker who crossed and uses min time to cross a bridge
			 */
			if (HikersWhoCrossedBridge & (1 << i))
			{
				const HikerInfo & hiker = HikersList.at(i);
				if (MinTimeOfCrossedHiker > hiker.GetTimeToCrossBridgeOnce())
				{
					HikerId = i;
					MinTimeOfCrossedHiker = hiker.GetTimeToCrossBridgeOnce();
					found = true;
				}
			}	
		}
		if (found)
		{
			/*
			 * This hiker is going back to help other hikers with torch
			 */

			// Recursively call to get min time
			MinTime = MinTimeOfCrossedHiker + 
				ComputeHikersTimeToCrossBridge((HikersCrossingInfo | (1 << HikerId)), 
							       (moveDirection ^ UNDO_CROSSING)); 
		}
		else
		{
			// Debugg info: Should not happen
			cerr << "ComputeHikersTimeToCrossBridge: Unexpected error" << endl;
		}	
	}
	else
	{
		int NumPersonNotCrossed = GetSetBitCounts (HikersCrossingInfo);
		if (NumPersonNotCrossed == 1)
		{
			/*
			 * There is one only person who need to cross
			 */
                        for (int i = 0; (i < NumHiker); ++i)
                        {
				if (HikersCrossingInfo && (1 << i))
				{
					HikerInfo & hiker = HikersList.at(i);
					MinTime = hiker.GetTimeToCrossBridgeOnce();
					break;					
				}
	         	}
		}
		else
		{
			/*
			 * Send hikers as pair
			 */

			MinTime = FLT_MAX; // Init to max value
			
			for (int i = 0; (i < NumHiker); ++i)
			{
				if (!(HikersCrossingInfo & (1 << i)))
				{
					// This hiker alreday crossed the bridge
					continue;
				}

				for (int j = i + 1; (j < NumHiker); ++j)
				{
					if (HikersCrossingInfo & (1 << j))
					{
						float SlowestTime = std::max(HikersList.at(i).GetTimeToCrossBridgeOnce(),
								      HikersList.at(j).GetTimeToCrossBridgeOnce());

						/*
						 * Unset i and j Hiker as they crossed the bridge
						 */
						int HikerNewCrossInfo = HikersCrossingInfo ^ (1 << i) ^ (1 << j);
						SlowestTime += ComputeHikersTimeToCrossBridge(HikerNewCrossInfo,
                                                            				      moveDirection ^ 1);
						/*
						 * Get min time among all times
						 */

						MinTime = std::min(MinTime, SlowestTime);
					}
				}
			}
		}	
	}

	return MinTime;
}

/*
 * This function will take input from live person who run this
 * program.
 */
static vector <HikerInfo> GetHikerInfoForBridge(const BridgeInfo & bridge)
{
	int NumHikers;
	vector<HikerInfo> Hikers;
	
	int id = bridge.GetBridgeId();

	if (id > 1)
	{
		cout << "Enter number of additionial hikers for the new bridge: ";
	}
	else
	{
		cout << "Enter Numbers hikers for the first bridge: ";
	}

	cin >> NumHikers;
	cin.ignore(numeric_limits<streamsize>::max(),'\n');

	for (auto i = 0; (i < NumHikers); ++i)
	{
		string HikerName;
		double HikerSpeed;

		cout << "Enter Hiker# " << i+1 << " Name: ";
		getline(cin, HikerName); 

		cout << "Enter Hiker# " << i+1 << " speed in ft/min: ";
		cin >>  HikerSpeed;
		cin.ignore(numeric_limits<streamsize>::max(),'\n');

		HikerInfo hi(HikerName, HikerSpeed, bridge.GetBridgeLength());
		Hikers.push_back(hi);
	}

	return Hikers;
}

#if 0

/*
 * This test driver is to take input (bridge and hikers' info) interactively
 * from live person who run this program.
 * Multiple bridges and hikers are suported with matrics from indiviidual and total
 * bridges and hikers.
 */
int main()
{
        BridgeAndHikersSolution solution( 1 << 20);
	int NumBridges;

	yaml_parser_t parser;
	cout << "Please Enter Number of Bridges: ";
	cin >> NumBridges;
	cin.ignore(numeric_limits<streamsize>::max(),'\n');

	for (auto i = 0; (i < NumBridges); ++i)
	{
		double BridgeLength;
		
		cout << "Please enter length of the Bridge: ";
		cin >> BridgeLength;

		BridgeInfo Bridge((i+1), BridgeLength);

		vector<HikerInfo> hikers = GetHikerInfoForBridge(Bridge);

		solution.ComputeTimeToCrossCurrentBridge(Bridge, hikers);

		//solution.PrintStatForABridge(i+1);
	}

	// Print crossing info of all bridges and hikers
	solution.PrintStatForAllBridge();
	return 0;
}

#endif
