/*
 * Copyright 2020 OmniSci, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "DataMgr/AbstractBufferMgr.h"
#include "DataMgr/FileMgr/GlobalFileMgr.h"
#include "DataMgr/ForeignStorage/ForeignStorageMgr.h"

using namespace Data_Namespace;

class PersistentStorageMgr : public AbstractBufferMgr {
 public:
#ifdef HAVE_DCPMM
  PersistentStorageMgr(const std::string& data_dir, const bool pmm_store, const std::string& pmm_store_path, const size_t num_reader_threads)
      : AbstractBufferMgr(0)
      , global_file_mgr(
            std::make_unique<File_Namespace::GlobalFileMgr>(0,
		                                            pmm_store,
							    pmm_store_path,
                                                            data_dir,
                                                            num_reader_threads))
      , foreign_storage_mgr(std::make_unique<foreign_storage::ForeignStorageMgr>()) {}

  AbstractBuffer* createBuffer(BufferProperty bufProp,
                               const ChunkKey& key,
                               const size_t maxRows,
                               const int sqlTypeSize,
                               const size_t page_size) override {
                                 AbstractBuffer *buffer = createBuffer(bufProp, key, page_size, maxRows * sqlTypeSize);
                                 buffer->setMaxRows(maxRows);
                                 return buffer;
                               }
#else /* HAVE_DCPMM */
  PersistentStorageMgr(const std::string& data_dir, const size_t num_reader_threads)
      : AbstractBufferMgr(0)
      , global_file_mgr(
            std::make_unique<File_Namespace::GlobalFileMgr>(0,
                                                            data_dir,
                                                            num_reader_threads))
      , foreign_storage_mgr(std::make_unique<foreign_storage::ForeignStorageMgr>()) {}
#endif /* HAVE_DCPMM */

  AbstractBuffer* createBuffer(BufferProperty bufProp,
                               const ChunkKey& chunk_key,
                               const size_t page_size,
                               const size_t initial_size) override;
  void deleteBuffer(const ChunkKey& chunk_key, const bool purge) override;
  void deleteBuffersWithPrefix(const ChunkKey& chunk_key_prefix,
                               const bool purge) override;
  AbstractBuffer* getBuffer(BufferProperty bufProp, const ChunkKey& chunk_key, const size_t num_bytes) override;
  void fetchBuffer(const ChunkKey& chunk_key,
                   AbstractBuffer* destination_buffer,
                   const size_t num_bytes) override;
  AbstractBuffer* putBuffer(const ChunkKey& chunk_key,
                            AbstractBuffer* source_buffer,
                            const size_t num_bytes) override;
  void getChunkMetadataVec(ChunkMetadataVector& chunk_metadata) override;
  void getChunkMetadataVecForKeyPrefix(ChunkMetadataVector& chunk_metadata,
                                       const ChunkKey& chunk_key_prefix) override;
#ifdef HAVE_DCPMM
  bool isBufferInPersistentMemory(const ChunkKey& chunk_key) override;
#endif /* HAVE_DCPMM */
  bool isBufferOnDevice(const ChunkKey& chunk_key) override;
  std::string printSlabs() override;
  void clearSlabs() override;
  size_t getMaxSize() override;
  size_t getInUseSize() override;
  size_t getAllocated() override;
  bool isAllocationCapped() override;
  void checkpoint() override;
  void checkpoint(const int db_id, const int tb_id) override;
  AbstractBuffer* alloc(const size_t num_bytes) override;
  void free(AbstractBuffer* buffer) override;
  MgrType getMgrType() override;
  std::string getStringMgrType() override;
  size_t getNumChunks() override;
  void removeTableRelatedDS(const int db_id, const int table_id) override;
  File_Namespace::GlobalFileMgr* getGlobalFileMgr();

 private:
  bool isForeignStorage(const ChunkKey& chunk_key);

  std::unique_ptr<File_Namespace::GlobalFileMgr> global_file_mgr;
  std::unique_ptr<foreign_storage::ForeignStorageMgr> foreign_storage_mgr;
};
