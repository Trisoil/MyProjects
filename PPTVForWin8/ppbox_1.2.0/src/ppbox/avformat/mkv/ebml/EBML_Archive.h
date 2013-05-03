// EBML_Archive.h

#ifndef _PPBOX_AVFORMAT_MKV_EBML_EBML_ARCHIVE_H_
#define _PPBOX_AVFORMAT_MKV_EBML_EBML_ARCHIVE_H_

#include "ppbox/avformat/mkv/ebml/EBML_Type.h"

namespace ppbox
{
    namespace avformat
    {

        class EBML_DataSizeArchive;
        class EBML_ElementIArchive;

    }
}

namespace util
{
    namespace serialization
    {

        template <>
        struct is_primitive<ppbox::avformat::EBML_DataSizeArchive, ppbox::avformat::EBML_ElementHeader>
            : boost::true_type
        {
        };

        template <
            typename _Ty, 
            typename _Traits
        >
        struct is_primitive<ppbox::avformat::EBML_DataSizeArchive, ppbox::avformat::EBML_UnitType<_Ty, _Traits> >
            : boost::true_type
        {
        };

        template <>
        struct is_primitive<ppbox::avformat::EBML_ElementIArchive, ppbox::avformat::EBML_ElementHeader>
            : boost::true_type
        {
        };

        template <
            typename _Ty, 
            typename _Traits
        >
        struct is_primitive<ppbox::avformat::EBML_ElementIArchive, ppbox::avformat::EBML_UnitType<_Ty, _Traits> >
            : boost::true_type
        {
        };

    }
}

namespace ppbox
{
    namespace avformat
    {

        // EBML_DataSizeArchive
        class EBML_DataSizeArchive
            : public util::archive::BasicIArchive<EBML_DataSizeArchive>
        {
        public:
            EBML_DataSizeArchive()
            {
            }

        public:
            template <typename _Ty>
            void load(
                _Ty const &)
            {
                sizes_.back() += sizeof(_Ty);
            }

            void load (
                EBML_ElementHeader & h)
            {
                headers_.back() = (EBML_ElementHeader *)&h;
                h.Size = 1; // make it not empty
            }

            template <
                typename _Ty, 
                typename _Traits
            >
            void load (
                EBML_UnitType<_Ty, _Traits> const & h)
            {
                sizes_.back() += h.byte_size();
            }

            void sub_start()
            {
                headers_.push_back(NULL);
                sizes_.push_back(0);
            }

            void sub_end()
            {
                headers_.back()->Size = sizes_.back();
                if (sizes_.back() > 0) {
                    sizes_.back() += headers_.back()->head_size();
                }
                headers_.pop_back();
                if (sizes_.size() > 1) {
                    sizes_[sizes_.size() - 2] += sizes_.back();
                }
                sizes_.pop_back();
            }

        private:
            std::vector<EBML_ElementHeader *> headers_;
            std::vector<boost::uint64_t> sizes_;
        };

        class EBML_ElementIArchive
            : public util::archive::BasicIArchive<EBML_ElementIArchive>
        {
        public:
            struct Element
                : EBML_ElementHeader
            {
                boost::uint8_t act;
                boost::uint64_t offset;
                std::vector<Element> items;
            };

        public:
            EBML_ElementIArchive(
                EBML_IArchive & ar)
                : ar_(ar)
            {
            }

            EBML_ElementIArchive(
                EBML_IArchive & ar, 
                boost::uint64_t size)
                : ar_(ar)
            {
                Element elem;
                elem.Size = size;
                elem.act = 0;
                elem.offset = 0;
                load_items(elem);
                elements_.push_back(elem);
            }

            EBML_ElementIArchive(
                EBML_ElementIArchive & ar)
                : ar_(ar.ar_)
            {
            }

        public:
            void skip(
                boost::uint32_t id)
            {
                skip_ids_.insert(id);
            }

            void raw_data(
                boost::uint32_t id)
            {
                raw_data_ids_.insert(id);
            }

            std::vector<Element> const & skip_elements() const
            {
                return skip_elements_;
            }

        public:
            template <typename EBML_Elem>
            EBML_ElementIArchive & operator >> (
                std::vector<EBML_Elem> & vec)
            {
                while (true) {
                    EBML_Elem item;
                    operator >>(item);
                    if (item.empty()) {
                        break;
                    }
                    vec.push_back(item);
                }
                return *this;
            }

            using util::archive::BasicIArchive<EBML_ElementIArchive>::operator >>;

        public:
            void load(
                EBML_ElementHeader & v)
            {
                v.clear();
                if (elements_.size() == 1) {
                    Element & elem = elements_.back();
                    elem.act = 0;
                    elem.offset = ar_.tellg();
                    ar_ >> v;
                    if (ar_) {
                        (EBML_ElementHeader &)elem = v;
                        load_items(elem);
                    }
                }
                Element & elem = elements_.back();
                Element & pelem = *(&elem - 1);
                for (size_t i = 0; i < pelem.items.size(); ++i) {
                    if (pelem.items[i].Id == v.Id) {
                        if (pelem.items[i].act == 1) {
                            break;
                        }
                        elem = pelem.items[i];
                        pelem.items.erase(pelem.items.begin() + i);
                        if (skip_ids_.find(v.Id) != skip_ids_.end()) {
                            skip_elements_.push_back(elem);
                            break;
                        }
                        v = elem;
                        if (raw_data_ids_.find(v.Id) == raw_data_ids_.end())
                            load_items(elem);
                        break;
                    }
                }
            }

            template <
                typename _Ty, 
                typename _Traits
            >
            void load(
                EBML_UnitType<_Ty, _Traits> & v)
            {
                v.clear();
                Element & elem = elements_.back();
                for (size_t i = 0; i < elem.items.size(); ++i) {
                    if (elem.items[i].Id == v.Id) {
                        Element & elem2 = elem.items[i];
                        ar_.seekg(elem2.offset + elem2.head_size(), std::ios::beg);
                        (EBML_ElementHeader &)v = elem2;
                        _Traits::load(ar_, v.value(), elem2.Size);
                    }
                }
            }

            template <
                typename _Ty
            >
            void load(
                _Ty & v)
            {
                ar_.load(v);
            }

        public:
            void sub_start()
            {
                elements_.push_back(Element());
            }

            void sub_end()
            {
                elements_.pop_back();
            }

        private:
            void load_items(
                Element & elem)
            {
                boost::uint64_t offset = elem.offset;
                boost::uint64_t end = offset + elem.byte_size();
                offset += elem.head_size();
                ar_.seekg(offset, std::ios::beg);
                EBML_ElementHeader h;
                while (offset < end && ar_ >> h) {
                    Element item;
                    (EBML_ElementHeader &)item = h;
                    item.act = 0;
                    item.offset = offset;
                    offset += h.byte_size();
                    elem.items.push_back(item);
                    ar_.seekg(h.data_size(), std::ios::cur);
                    if (!ar_) {
                        break;
                    }
                }
                ar_.clear();
            }

        private:
            EBML_IArchive ar_;
            std::vector<Element> elements_;
            std::set<boost::uint32_t> skip_ids_;
            std::set<boost::uint32_t> raw_data_ids_;
            std::vector<Element> skip_elements_;
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_MKV_EBML_EBML_ARCHIVE_H_
