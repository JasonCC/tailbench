/** @file flint_metadata.h
 * @brief Access to metadata for a flint database.
 */
/* Copyright (C) 2005,2007,2008 Olly Betts
 * Copyright (C) 2008 Lemur Consulting Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef XAPIAN_INCLUDED_FLINT_METADATA_H
#define XAPIAN_INCLUDED_FLINT_METADATA_H

#include <xapian/base.h>
#include <xapian/database.h>
#include <xapian/types.h>

#include "alltermslist.h"
#include "flint_table.h"
#include "termlist.h"

#include <string>

class FlintCursor;

class FlintMetadataTermList : public AllTermsList {
    /// Copying is not allowed.
    FlintMetadataTermList(const FlintMetadataTermList &);

    /// Assignment is not allowed.
    void operator=(const FlintMetadataTermList &);

    /// Keep a reference to our database to stop it being deleted.
    Xapian::Internal::RefCntPtr<const Xapian::Database::Internal> database;

    /** A cursor which runs through the postlist table reading metadata keys.
     */
    FlintCursor * cursor;

    /** The prefix that all returned keys must have.
     */
    std::string prefix;

  public:
    FlintMetadataTermList(Xapian::Internal::RefCntPtr<const Xapian::Database::Internal> database_,
			  FlintCursor * cursor_, const std::string &prefix_);

    ~FlintMetadataTermList();

    /** Returns the current termname.
     *
     *  Either next() or skip_to() must have been called before this
     *  method can be called.
     */
    std::string get_termname() const;

    /** Return the term frequency for the term at the current position.
     *
     *  Not meaningful for a FlintMetadataTermList.
     */
    Xapian::doccount get_termfreq() const;

    /** Return the collection frequency for the term at the current position.
     *
     *  Not meaningful for a FlintMetadataTermList.
     */
    Xapian::termcount get_collection_freq() const;

    /// Advance to the next term in the list.
    TermList * next();

    /// Advance to the first key which is >= key.
    TermList * skip_to(const std::string &key);

    /// True if we're off the end of the list
    bool at_end() const;
};

#endif // XAPIAN_INCLUDED_FLINT_METADATA_H
