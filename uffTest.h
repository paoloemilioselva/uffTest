#ifndef UFFTEST_H
#define UFFTEST_H

#include <pxr/pxr.h>
#include <pxr/usd/sdf/api.h>
#include <pxr/usd/sdf/abstractData.h>
#include <pxr/usd/sdf/fileFormat.h>
#include <pxr/base/tf/declarePtrs.h>
#include <pxr/base/tf/token.h>
#include <pxr/base/vt/value.h>
#include <pxr/usd/pcp/dynamicFileFormatInterface.h>
#include <pxr/base/tf/staticTokens.h>

PXR_NAMESPACE_USING_DIRECTIVE

#define UFF_TOKENS \
    ( ( Id, "uff" ) )\
    ( ( Version, "1.0" ) )\
    ( ( Target, "usd" ) ) \
    ( ( Extension, "uff" ) ) \
    ( ( Params, "uffParams" ) )
TF_DECLARE_PUBLIC_TOKENS(uffTokens, UFF_TOKENS);

#define UFF_PARAM_NAMES \
    ( ( cubesX, "cubesX" ) )\
    ( ( cubesY, "cubesY" ) )\
    ( ( cubesZ, "cubesZ" ) )\
    ( ( randomRotY, "randomRotY" ) )
TF_DECLARE_PUBLIC_TOKENS(uffParamNames, UFF_PARAM_NAMES);

TF_DECLARE_WEAK_AND_REF_PTRS(uffTest);

class uffTest : public pxr::SdfFileFormat, public pxr::PcpDynamicFileFormatInterface
{
public:
    virtual bool CanRead(const std::string& i_file) const override;
    
    virtual bool Read(pxr::SdfLayer* io_layer,
        const std::string& i_resolvedPath,
        bool i_metadataOnly) const override;
    
    virtual bool ReadFromString(pxr::SdfLayer* layer,
        const std::string& str) const override;

    virtual void ComposeFieldsForFileFormatArguments(
        const std::string& assetPath,
        const pxr::PcpDynamicFileFormatContext& context,
        pxr::SdfFileFormat::FileFormatArguments* args,
        pxr::VtValue* contextDependencyData) const override;

    virtual bool WriteToString(const pxr::SdfLayer& layer,
        std::string* str,
        const std::string& comment = std::string()) const override;

    virtual bool WriteToStream(const pxr::SdfSpecHandle& spec,
        std::ostream& out,
        size_t indent) const override;

protected:
    SDF_FILE_FORMAT_FACTORY_ACCESS;
    
    bool _ShouldSkipAnonymousReload() const override;
    bool _ShouldReadAnonymousLayers() const override;

    virtual ~uffTest();
    uffTest();
};

#endif