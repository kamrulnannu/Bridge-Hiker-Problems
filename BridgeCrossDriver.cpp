#include <yaml.h>
#include <iostream>
#include <stdio.h>
#include "BridgeCrossing.h"

using namespace std;

enum
{
	BRIDGE_HIKER = 0,
	BRIDGE = 1,
	HIKER = 2
};

// Prototype
static bool ReadConfigFile (BridgeAndHikersSolution &solution,  FILE *fp);
static void ParseNext (yaml_parser_t &parser, 
		       yaml_event_t &event);
static void ProcessEvent (yaml_parser_t &parser,  
		         yaml_event_t &event, 
			 BridgeAndHikersSolution &solution);
static void GetBridgeHikerInfo (yaml_parser_t &parser,
		                yaml_event_t &event, 
	                        BridgeAndHikersSolution &solution);
static void GetHikers (yaml_parser_t &parser, 
		      yaml_event_t &event,
		      vector<HikerInfo> &Hikers,
		      const BridgeInfo &theBridgea);

// Implementation
static void ParseNext(yaml_parser_t &parser,  yaml_event_t &event)
{
/* Parse next scalar. if wrong exit with error */
    if (!yaml_parser_parse(&parser, &event)) 
    {
    	cerr << "Error: ParseNext - Parse error, " << parser.error << endl;
        exit(1);
    }	
}

static void GetNextEvent(yaml_parser_t &parser,  yaml_event_t &event)
{
	 yaml_event_delete(&event);
         ParseNext(parser, event);
}

static void GetBridge(yaml_parser_t &parser, 
		      yaml_event_t &event, 
		      BridgeInfo &Bridge)
{
	enum NextBrdigeEvent
	{
		NOT_VALID_BRIDGE_EVENT = 0,
		BRIDGE_EVENT_ID = 1,
		BRIDGE_EVENT_LEN = 2
	};

	char *buf = (char *)event.data.scalar.value;
	
	NextBrdigeEvent NBE = NOT_VALID_BRIDGE_EVENT;

 	if ((event.type == YAML_SCALAR_EVENT) &&
	    (strcmp(buf, "Bridge") == 0))
	{
		// Delete cur event and move to next
		GetNextEvent(parser, event);

		// Delete cur event and move to next
		GetNextEvent(parser, event);

		while (event.type == YAML_SCALAR_EVENT)
		{
			char *buf = (char *)event.data.scalar.value;
		        if (strcmp(buf, "id") == 0)
			{
				GetNextEvent(parser, event);
				NBE = BRIDGE_EVENT_ID;		
			}
			else if (strcmp(buf, "length") == 0)
			{
				GetNextEvent(parser, event);
				NBE = BRIDGE_EVENT_LEN;
			}
			else if (NBE == BRIDGE_EVENT_ID)
			{
				int id = atoi(buf);
				Bridge.SetBridgeId(id);
				NBE = NOT_VALID_BRIDGE_EVENT;
				GetNextEvent(parser, event);
			}
			else if (NBE == BRIDGE_EVENT_LEN)
			{
				int len = atoi(buf);
				Bridge.SetBridgeLength(len);
				NBE = NOT_VALID_BRIDGE_EVENT;
				GetNextEvent(parser, event);
			}
			else if (NBE != BRIDGE_EVENT_ID)
			{
				cerr << "Error: GetBridge, incorrect entry config file." << endl;
				exit(1);
			}	
		}

		// Delete YAML_MAPPING_END_EVENT and move to next
                GetNextEvent(parser, event);
	}
	else
	{
		cerr << "Error: GetBridge, incorrect config file." << endl;
		exit(1);
	}	
}

static void GetHikers(yaml_parser_t &parser, 
		      yaml_event_t &event,
		      vector<HikerInfo> &Hikers,
		      const BridgeInfo &theBridge)
{
	// Delete YAML_MAP_START_EVENT and move to YAML_SCALAR_EVENT
	GetNextEvent(parser, event);
	
	char *name = nullptr;
	int speed = 0;
	string strName = "";

	while (event.type == YAML_SCALAR_EVENT)
	{
		char *item = (char *)event.data.scalar.value;

		if (strcmp(item, "-name") == 0)
		{
			// Delete cur event and get val for -name
			GetNextEvent(parser, event);
			if (event.type == YAML_SCALAR_EVENT)
			{
				name = (char *)event.data.scalar.value;
				strName = name;
				name = nullptr;
			}
			else
			{
				cerr <<"Error: GetHikers - Invalid entry for name-val in config file."<<endl;
				exit(1);
			}
		}
		else if (strcmp(item, "speed") == 0)
		{
			// Delete cur event and get val for speed
                        GetNextEvent(parser, event);
			if (event.type == YAML_SCALAR_EVENT)
			{
				item = (char *)event.data.scalar.value;
				speed = atoi(item);
			}
                        else
                        {
                                cerr <<"Error: GetHikers - Invalid entry for speed-val in config file."<<endl;
                                exit(1);
                        }
		}

		if (strName.length() && speed)
		{
			HikerInfo aHiker(strName, speed, theBridge.GetBridgeLength());
			Hikers.push_back(aHiker);
			name = nullptr;
			speed = 0;
			strName = "";
		}
		
		/*
		 * Delete name val or speed val event and
		 * move to -name tag
		 */		
		GetNextEvent(parser, event);
	}
}

static void GetBridgeHikerInfo (yaml_parser_t &parser, 
		                yaml_event_t &event, 
		                BridgeAndHikersSolution &solution)
{
	char *item = (char *)event.data.scalar.value;
	
	BridgeAndHikersInfo bhIfo;
	BridgeInfo aBridge;
	vector<HikerInfo> Hikers;
	bool bBridgeProcessed = false;
	
	if (strcmp(item, "BridgeAndHikers") == 0)
	{
		// Delete BridgeAndHikers event and move next
		GetNextEvent(parser, event);

		// Delete MAR_START_EVENT and move YAML_SCALAR_EVENT
		GetNextEvent(parser, event);

		if (event.type == YAML_SCALAR_EVENT)
		{
			GetBridge(parser, event, aBridge);
			bBridgeProcessed = true;
		}

		item = (char *)event.data.scalar.value;
		if (event.type == YAML_SCALAR_EVENT)
		{
			if (strcmp(item, "Hikers") == 0)
			{
				// Delete "Hikers" event and move YAML_SCALAR_EVENT
				// of hiker's name and speed tag
				GetNextEvent(parser, event);
				Hikers.clear();
				GetHikers(parser, event, Hikers, aBridge);			
			}
			if (Hikers.size() == 0)
			{
				/*
				 * If no hiker for a given bridge, we need to make
				 * sure that there are some hikers from earlier
				 * bridges
				 */
				if (solution.GetNumBridge() == 0)
				{
					cerr <<"Error-0: GetBridgeHikerInfo - There should be some hikers in the first entry of bridge."<<endl;
					exit(1);
				}
			}
			//BridgeAndHikersInfo bhIfo(aBridge, Hikers);
			
			solution.ComputeTimeToCrossCurrentBridge(aBridge, Hikers);
		}
		else if (bBridgeProcessed)
		{
			// There is no entry in config file Hikers for the given bridge
			if (Hikers.size() == 0)
			{
				/*
				 * If no hiker for a given bridge, we need to make
				 * sure that there are some hikers from earlier
				 * bridges
				 */
				if (solution.GetNumBridge() == 0)
				{
					cerr <<"Error-1: GetBridgeHikerInfo - There should be some hikers in the first entry of bridge."<<endl;
					exit(1);
				}
			}
			solution.ComputeTimeToCrossCurrentBridge(aBridge, Hikers);
			bBridgeProcessed = false;
		}
	}
}

static void ProcessEvent(yaml_parser_t &parser,  yaml_event_t &event, BridgeAndHikersSolution &solution)
{
	char *buf = (char *)event.data.scalar.value;

	switch (event.type)
	{
		case YAML_STREAM_START_EVENT:
            		break;

        	case YAML_STREAM_END_EVENT:
            		break;

        	case YAML_DOCUMENT_START_EVENT:
            		break;

        	case YAML_DOCUMENT_END_EVENT:
           		break;

        	case YAML_SEQUENCE_START_EVENT:
            		break;

        	case YAML_SEQUENCE_END_EVENT:
            		break;

        	case YAML_MAPPING_START_EVENT:
            		break;

        	case YAML_ALIAS_EVENT:
			cerr << "Error: ProcessEvent - got alias anchor=" 
			     << event.data.alias.anchor << endl;
            		exit(1);
           		break;

        	case YAML_SCALAR_EVENT:
			GetBridgeHikerInfo (parser, event, solution);
            		break;

		case YAML_NO_EVENT:
			cerr << "Error: ProcessEvent - No Event" << endl;
            		exit(1);
            	break;
	}
}

static bool ReadConfigFile (BridgeAndHikersSolution &solution,  FILE *fp)
{
	yaml_parser_t parser;
	yaml_event_t event;

	if(!yaml_parser_initialize(&parser))
        {
                cout << "Failed to initialize parser!" << endl;

		exit(1);
        }

	/* Set input file */
 	yaml_parser_set_input_file(&parser, fp);

	do
	{
		ParseNext(parser, event);

		ProcessEvent(parser, event, solution);

		if (event.type != YAML_STREAM_END_EVENT ) 
		{
            		yaml_event_delete( &event);
		}

	} while(event.type != YAML_STREAM_END_EVENT );
	
	
	// Done
	yaml_parser_delete(&parser);
	
	return true;
}

int main(int argc, char **argv)
{
        BridgeAndHikersSolution solution( 1 << 20);

	if (2 != argc)
	{
		cerr <<"YAML confile missing."<<endl;
		cerr <<"Usage: " << argv[0] <<": inputfile.yaml" << endl;
		exit(1);
	}
	FILE * fp = fopen(argv[1], "r");

	if (!fp)
	{
		cout << "Error: Failed to open file for reading, FileName="<< argv[1] << endl;
		exit (1);
	}

	// Code here to read from yaml file
	ReadConfigFile(solution, fp);
	
	solution.PrintStatForAllBridge();

	fclose(fp);
	return 0;
}
