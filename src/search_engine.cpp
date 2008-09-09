/***************************************************************************
 *   Copyright (C) 2008 by Andrzej Rybczak   *
 *   electricityispower@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <algorithm>
#include "search_engine.h"

extern MPDConnection *Mpd;
extern Menu<Song> *mPlaylist;
extern Menu<string> *mSearcher;

bool search_match_to_pattern = 0;
bool search_case_sensitive = 1;

void UpdateFoundList(const SongList &v)
{
	int i = search_engine_static_option;
	bool bold = 0;
	for (SongList::const_iterator it = v.begin(); it != v.end(); it++, i++)
	{
		for (int j = 0; j < mPlaylist->Size(); j++)
		{
			if (mPlaylist->at(j).GetHash() == (*it)->GetHash())
			{
				bold = 1;
				break;
			}
		}
		mSearcher->BoldOption(i, bold);
		bold = 0;
	}
}

void PrepareSearchEngine(Song &s)
{
	s.Clear();
	mSearcher->Clear();
	mSearcher->Reset();
	mSearcher->AddOption("[.b]Filename:[/b] " + s.GetShortFilename());
	mSearcher->AddOption("[.b]Title:[/b] " + s.GetTitle());
	mSearcher->AddOption("[.b]Artist:[/b] " + s.GetArtist());
	mSearcher->AddOption("[.b]Album:[/b] " + s.GetAlbum());
	mSearcher->AddOption("[.b]Year:[/b] " + s.GetYear());
	mSearcher->AddOption("[.b]Track:[/b] " + s.GetTrack());
	mSearcher->AddOption("[.b]Genre:[/b] " + s.GetGenre());
	mSearcher->AddOption("[.b]Comment:[/b] " + s.GetComment());
	mSearcher->AddSeparator();
	mSearcher->AddOption("[.b]Search mode:[/b] " + (search_match_to_pattern ? search_mode_normal : search_mode_strict));
	mSearcher->AddOption("[.b]Case sensitive:[/b] " + (string)(search_case_sensitive ? "Yes" : "No"));
	mSearcher->AddSeparator();
	mSearcher->AddOption("Search");
	mSearcher->AddOption("Reset");
}

void Search(SongList &result, Song &s)
{
	FreeSongList(result);
	
	if (s.Empty())
		return;
	
	SongList list;
	Mpd->GetDirectoryRecursive("/", list);
	
	bool found = 1;
	
	s.GetEmptyFields(1);
	
	if (!search_case_sensitive)
	{
		string t;
		t = s.GetShortFilename();
		transform(t.begin(), t.end(), t.begin(), tolower);
		s.SetShortFilename(t);
		
		t = s.GetTitle();
		transform(t.begin(), t.end(), t.begin(), tolower);
		s.SetTitle(t);
		
		t = s.GetArtist();
		transform(t.begin(), t.end(), t.begin(), tolower);
		s.SetArtist(t);
		
		t = s.GetAlbum();
		transform(t.begin(), t.end(), t.begin(), tolower);
		s.SetAlbum(t);
		
		t = s.GetGenre();
		transform(t.begin(), t.end(), t.begin(), tolower);
		s.SetGenre(t);
		
		t = s.GetComment();
		transform(t.begin(), t.end(), t.begin(), tolower);
		s.SetComment(t);
	}
	
	for (SongList::const_iterator it = list.begin(); it != list.end(); it++)
	{
		Song copy = **it;
		
		if (!search_case_sensitive)
		{
			string t;
			t = copy.GetShortFilename();
			transform(t.begin(), t.end(), t.begin(), tolower);
			copy.SetShortFilename(t);
		
			t = copy.GetTitle();
			transform(t.begin(), t.end(), t.begin(), tolower);
			copy.SetTitle(t);
		
			t = copy.GetArtist();
			transform(t.begin(), t.end(), t.begin(), tolower);
			copy.SetArtist(t);
		
			t = copy.GetAlbum();
			transform(t.begin(), t.end(), t.begin(), tolower);
			copy.SetAlbum(t);
		
			t = copy.GetGenre();
			transform(t.begin(), t.end(), t.begin(), tolower);
			copy.SetGenre(t);
		
			t = copy.GetComment();
			transform(t.begin(), t.end(), t.begin(), tolower);
			copy.SetComment(t);
		}
		
		if (search_match_to_pattern)
		{
			if (found && !s.GetShortFilename().empty())
				found = copy.GetShortFilename().find(s.GetShortFilename()) != string::npos;
			if (found && !s.GetTitle().empty())
				found = copy.GetTitle().find(s.GetTitle()) != string::npos;
			if (found && !s.GetArtist().empty())
				found = copy.GetArtist().find(s.GetArtist()) != string::npos;
			if (found && !s.GetAlbum().empty())
				found = copy.GetAlbum().find(s.GetAlbum()) != string::npos;
			if (found && !s.GetYear().empty())
				found = StrToInt(copy.GetYear()) == StrToInt(s.GetYear()) && StrToInt(s.GetYear());
			if (found && !s.GetTrack().empty())
				found = StrToInt(copy.GetTrack()) == StrToInt(s.GetTrack()) && StrToInt(s.GetTrack());
			if (found && !s.GetGenre().empty())
				found = copy.GetGenre().find(s.GetGenre()) != string::npos;
			if (found && !s.GetComment().empty())
				found = copy.GetComment().find(s.GetComment()) != string::npos;
		}
		else
		{
			if (found && !s.GetShortFilename().empty())
				found = copy.GetShortFilename() == s.GetShortFilename();
			if (found && !s.GetTitle().empty())
				found = copy.GetTitle() == s.GetTitle();
			if (found && !s.GetArtist().empty())
				found = copy.GetArtist() == s.GetArtist();
			if (found && !s.GetAlbum().empty())
				found = copy.GetAlbum() == s.GetAlbum();
			if (found && !s.GetYear().empty())
				found = StrToInt(copy.GetYear()) == StrToInt(s.GetYear()) && StrToInt(s.GetYear());
			if (found && !s.GetTrack().empty())
				found = StrToInt(copy.GetTrack()) == StrToInt(s.GetTrack()) && StrToInt(s.GetTrack());
			if (found && !s.GetGenre().empty())
				found = copy.GetGenre() == s.GetGenre();
			if (found && !s.GetComment().empty())
				found = copy.GetComment() == s.GetComment();
		}
		
		if (found)
		{
			Song *ss = new Song(**it);
			result.push_back(ss);
		}
		
		found = 1;
	}
	FreeSongList(list);
	s.GetEmptyFields(0);
}
