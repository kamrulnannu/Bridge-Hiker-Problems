#ifndef BRIDGECROSSING_H_INCLUDED
#define BRIDGECROSSING_H_INCLUDED

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Information of a given Hiker
class HikerInfo
{
	private: 
		string HikerName; 
		float HikerSpeed;
		float  TimeToCrossABridgeOnce;

	public:
	HikerInfo(const string name = "", const float speed = 0, const float BridgeLen = 0) :
		  HikerName(name),
		  HikerSpeed(speed), 
		  TimeToCrossABridgeOnce (0) 
	{
		if ((speed > 0) && (BridgeLen > 0))
		{
			TimeToCrossABridgeOnce = BridgeLen/speed;
		}
	}

	HikerInfo (const HikerInfo & rhs)
	{
		HikerName = rhs.HikerName;
		HikerSpeed = rhs.HikerSpeed;
		TimeToCrossABridgeOnce = rhs.TimeToCrossABridgeOnce;
	}

	HikerInfo & operator = (const HikerInfo & rhs)
	{
		HikerName = rhs.HikerName;
		HikerSpeed = rhs.HikerSpeed;
		TimeToCrossABridgeOnce = rhs.TimeToCrossABridgeOnce;

		return *this;
	}

	float GetTimeToCrossBridgeOnce() const
	{
		return TimeToCrossABridgeOnce;
	}

	void InitHikerTimeToCrossNewBridge(int BridgeLen)
	{
		if ((HikerSpeed > 0.0) && (BridgeLen > 0.0))
		{
			TimeToCrossABridgeOnce = BridgeLen/HikerSpeed;
		}
		else
		{
			// DEBUG INFO// DEBUG INFO
                        cerr << "Invalid input for speed and bridge len, Speed = "
                             << HikerSpeed << ", BridgeLen = " << BridgeLen << endl;
                        exit(1);
		}
	}

	void PrintHikerTimeInfo() const
	{
		cout << "Name = " << HikerName
		     <<	", Speed = " <<  HikerSpeed
		     << ", TimeToCross the Bridge Once = " << TimeToCrossABridgeOnce
		     << endl;
	}
};

// Information of a Bridge
class BridgeInfo
{
	private:
		int BridgeId;
		float BridgeLength;

	public:
		BridgeInfo(const int id = 0, const float len = 0) : 
			   BridgeId(id), BridgeLength(len)
	        {}

		BridgeInfo & operator = (const BridgeInfo & rhs)
		{
			BridgeId = rhs.BridgeId;
			BridgeLength = rhs.BridgeLength;

			return *this;
		}

		void PrintBridgeInfo() const
		{
			cout << endl << "=======BridgeInfo:" << "BridgeID=" << BridgeId << ", Bridge Length=" 
			     << BridgeLength << "=======" << endl;
		}	

		void SetBridgeId(int id)
		{
			BridgeId = id;
		}

		int GetBridgeId() const
		{
			return BridgeId;
		}

		void SetBridgeLength(const float len)
		{
			BridgeLength = len;
		}

		float GetBridgeLength() const
		{
			return BridgeLength;
		}
};

// Information of Hikers who cross a given Bridge
class BridgeAndHikersInfo
{
	private:

	// Information of a Bridge
	BridgeInfo aBridge;

	// Information of Hikers who will cross the above Bridge: aBridge
	vector<HikerInfo> HikersList;

	float MinTimeToCross;

	public:
	BridgeAndHikersInfo() : MinTimeToCross(0) {}
	
	BridgeAndHikersInfo(const BridgeInfo & bInfo) : MinTimeToCross(0) 
	{
		 aBridge = bInfo;
	}

	BridgeAndHikersInfo(const BridgeInfo & bInfo, const vector<HikerInfo> hikerList)
	{
		aBridge = bInfo;
		HikersList = hikerList;
		MinTimeToCross = 0;
	}

	void SetBridgeInfo( const BridgeInfo & Bridge)
	{
		aBridge = Bridge;
	}

	void AddaHikeInfo(const HikerInfo & hiker)
	{
		HikersList.push_back(hiker);
	}

	vector<HikerInfo> & GetHikers()
	{
		return HikersList;
	}
	
	void SetMinTimeToCross(float time)
	{
		MinTimeToCross = time;
	}

	void SetHikers(const vector<HikerInfo> & hList)
	{
		 HikersList = hList;
	}

	void InitHikerInfoForNewBridge()
	{
		for(size_t i = 0; (i < HikersList.size()); ++i) 
		{
			HikerInfo &hiker = HikersList.at(i);
			hiker.InitHikerTimeToCrossNewBridge(aBridge.GetBridgeLength());
		}

		// Lmabda for sorting by hiker cross time
		auto SortByTime = [] (const HikerInfo & hi1, const HikerInfo & hi2)
		{
			return (hi1.GetTimeToCrossBridgeOnce() < hi2.GetTimeToCrossBridgeOnce());
		};

		// Sort list of hikers by speed
		sort(HikersList.begin(), HikersList.end(), SortByTime);
	}

	float GetTotalTimeToCrossThisBridge() const
	{
		return MinTimeToCross;
	}

	void PrintaBridgeAndHikerInfo() const
	{
		aBridge.PrintBridgeInfo();
		cout <<"Number Of Hikers crossed the above bridge=" << HikersList.size()<<":"<<endl<<endl;
		for (auto hiker : HikersList)
		{
			hiker.PrintHikerTimeInfo();
		}
		cout << "Time taken to cross Bridge by above Hikers="<< MinTimeToCross << endl;
	}

	int GetNumHikersForThisBridge() const
	{
		return HikersList.size();
	}
};

class BridgeAndHikersSolution
{
	private:

	// Info of all Bridges and Hikers
	vector<BridgeAndHikersInfo> AllBridgeAndHikersList;
        int HikerMoveDirSize;
	float **HikerMoveDirection;	

	// Return the bridge which is currently crossing by Hikers
        BridgeAndHikersInfo & GetCurrentBridgeCrossInProgress()
        {
                return AllBridgeAndHikersList.at(AllBridgeAndHikersList.size() - 1);
        }
	
	void ComputeHikersTimeToCrossBridge(BridgeAndHikersInfo & bhInfo);
	float ComputeHikersTimeToCrossBridge(int HikersCrossingInfo,
			                   BridgeAndHikersInfo & bhInfo,
					   bool moveDirection,
                                           int NumHiker);

	// Prevent copy
	BridgeAndHikersSolution(const BridgeAndHikersSolution & rhs);
	BridgeAndHikersSolution & operator = (const BridgeAndHikersSolution &rhs);

	public:

	enum /* CrossDirection */
	{
		CROSS_BRIDGE = 0,  // Hiker in Crossed state 
		UNDO_CROSSING = 1  // Hiker Crossed but will go back with torch to help other hiker for crossing
	};
	BridgeAndHikersSolution(int MarkArraySize);
	
	~BridgeAndHikersSolution();

	void AddNewBridgeAndHikersInfo(const BridgeAndHikersInfo & aNewBridgeAndHikers)
	{
		AllBridgeAndHikersList.push_back(aNewBridgeAndHikers);
	}

	float ComputeTotalTimeOfCrossingForAllBridges() const
	{
		float time = 0;

		for (auto HB : AllBridgeAndHikersList)
		{
			time += HB.GetTotalTimeToCrossThisBridge();
		}

		return time;
	}
	
	int GetNumBridge() const
	{
		return AllBridgeAndHikersList.size();
	}
	
	// This will print stat for one bridge only
	void PrintStatForABridge(int BridgeId) const
	{
		if ((BridgeId < 1) && (BridgeId > AllBridgeAndHikersList.size()))
		{
			cerr << "Invalid Bridge ID = " << BridgeId
			     << ", Valid Ids are 1 - " << AllBridgeAndHikersList.size()
			     << endl;
			return;
		}
		AllBridgeAndHikersList.at(BridgeId-1).PrintaBridgeAndHikerInfo();
	}

	// This will Print Stat for all Bridges
	void PrintStatForAllBridge() const
	{
		for (auto HB : AllBridgeAndHikersList)
		{
			HB.PrintaBridgeAndHikerInfo();
		}

		cout << endl << "Summary: Number Of Bridges = " << GetNumBridge()
		     << ", Total time taken = " << ComputeTotalTimeOfCrossingForAllBridges()
		     << endl;
	}

	void ComputeTimeToCrossCurrentBridge(const BridgeInfo & bridge, 
			                     const vector<HikerInfo> &hikers);

	/*
	int ComputeHikersTimeToCrossBridge(int HikersCrossingInfo,
			                   BridgeAndHikersInfo & bhInfo,
					   bool moveDirection,
                                           int NumHiker);
        */
};

#endif
