/*
 Copyright (c) 2013 yvt
 
 This file is part of OpenSpades.
 
 OpenSpades is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 OpenSpades is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with OpenSpades.  If not, see <http://www.gnu.org/licenses/>.
 
 */

#include "Bitmap.h"
#include "IStream.h"
#include <vector>
#include "Exception.h"
#include "../Core/Debug.h"
#include "Debug.h"
#include "IBitmapCodec.h"
#include "FileManager.h"

namespace spades {
	Bitmap::Bitmap(int ww, int hh):
	w(ww), h(hh){
		SPADES_MARK_FUNCTION();
		
		pixels = new uint32_t[w * h];
		SPAssert(pixels != NULL);
	}
	
	Bitmap::~Bitmap() {
		SPADES_MARK_FUNCTION();
		
		delete[] pixels;
	}
	
	Bitmap *Bitmap::Load(const std::string& filename) {
		std::vector<IBitmapCodec *>codecs = IBitmapCodec::GetAllCodecs();
		std::string errMsg;
		for(size_t i = 0; i < codecs.size(); i++){
			IBitmapCodec *codec = codecs[i];
			if(codec->CanLoad() && codec->CheckExtension(filename)){
				// give it a try.
				// open error shouldn't be handled here
				StreamHandle str = FileManager::OpenForReading(filename.c_str());
				try{
					return codec->Load(str);
				}catch(const std::exception& ex){
					errMsg += codec->GetName();
					errMsg += ":\n";
					errMsg += ex.what();
					errMsg += "\n\n";
				}
			}
		}
		
		if(errMsg.empty()){
			SPRaise("Bitmap codec not found for filename: %s", filename.c_str());
		}else{
			SPRaise("No bitmap codec could load file successfully: %s\n%s\n",
					filename.c_str(), errMsg.c_str());
		}
	}
	
	void Bitmap::Save(const std::string &filename) {
		std::vector<IBitmapCodec *>codecs = IBitmapCodec::GetAllCodecs();
		for(size_t i = 0; i < codecs.size(); i++){
			IBitmapCodec *codec = codecs[i];
			if(codec->CanSave() && codec->CheckExtension(filename)){
				StreamHandle str = FileManager::OpenForWriting(filename.c_str());
				
				codec->Save(str, this);
				return;
			}
		}
		
		SPRaise("Bitmap codec not found for filename: %s", filename.c_str());
		
	}
	
}
