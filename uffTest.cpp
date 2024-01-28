#include "uffTest.h"

#include <pxr/usd/usd/usdaFileFormat.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/primSpec.h>
#include <pxr/usd/sdf/valueTypeName.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/base/tf/fileUtils.h>
#include <pxr/base/tf/registryManager.h>
#include <pxr/usd/pcp/dynamicFileFormatContext.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>

#include <iostream>

PXR_NAMESPACE_USING_DIRECTIVE

TF_DEFINE_PUBLIC_TOKENS(uffTokens, UFF_TOKENS);
TF_DEFINE_PUBLIC_TOKENS(uffParamNames, UFF_PARAM_NAMES);

TF_REGISTRY_FUNCTION(TfType) 
{
    SDF_DEFINE_FILE_FORMAT(uffTest, SdfFileFormat);
}

uffTest::uffTest() 
    : SdfFileFormat(
        uffTokens->Id, 
        uffTokens->Version,
        uffTokens->Target, 
        uffTokens->Extension)
{ 
}

uffTest::~uffTest()
{
}

bool uffTest::CanRead(const std::string& i_file) const 
{
    return true;
}

void AddMeshCube(pxr::UsdStageRefPtr i_stage, pxr::SdfPath& i_path, pxr::GfVec3d& i_pos, float i_rot)
{
    pxr::UsdGeomMesh cubeMesh = pxr::UsdGeomMesh::Define(i_stage, i_path);
    cubeMesh.CreateOrientationAttr().Set(pxr::UsdGeomTokens->leftHanded);
    cubeMesh.CreateSubdivisionSchemeAttr().Set(pxr::UsdGeomTokens->none);

    pxr::VtArray<pxr::GfVec3f> points;
    points.emplace_back(pxr::GfVec3f(0.5, -0.5, 0.5));
    points.emplace_back(pxr::GfVec3f(-0.5, -0.5, 0.5));
    points.emplace_back(pxr::GfVec3f(0.5, 0.5, 0.5));
    points.emplace_back(pxr::GfVec3f(-0.5, 0.5, 0.5));
    points.emplace_back(pxr::GfVec3f(-0.5, -0.5, -0.5));
    points.emplace_back(pxr::GfVec3f(0.5, -0.5, -0.5));
    points.emplace_back(pxr::GfVec3f(-0.5, 0.5, -0.5));
    points.emplace_back(pxr::GfVec3f(0.5, 0.5, -0.5));
    cubeMesh.CreatePointsAttr().Set(points);

    pxr::VtArray<pxr::GfVec3f> displayColors;
    displayColors.emplace_back(pxr::GfVec3f(1, 0, 1));
    displayColors.emplace_back(pxr::GfVec3f(0, 0, 1));
    displayColors.emplace_back(pxr::GfVec3f(1, 1, 1));
    displayColors.emplace_back(pxr::GfVec3f(0, 1, 1));
    displayColors.emplace_back(pxr::GfVec3f(0, 0, 0));
    displayColors.emplace_back(pxr::GfVec3f(1, 0, 0));
    displayColors.emplace_back(pxr::GfVec3f(0, 1, 0));
    displayColors.emplace_back(pxr::GfVec3f(1, 1, 0));
    pxr::UsdAttribute displayColorsAttr = cubeMesh.CreateDisplayColorAttr();
    displayColorsAttr.Set(displayColors);
    pxr::UsdGeomPrimvar displayColorsPrimvar(displayColorsAttr);
    displayColorsPrimvar.SetInterpolation(pxr::UsdGeomTokens->vertex);

    pxr::VtArray<int> faceVertexCounts = { 4, 4, 4, 4, 4, 4 };
    cubeMesh.CreateFaceVertexCountsAttr().Set(faceVertexCounts);
    pxr::VtArray<int> faceVertexIndices = { 0, 1, 3, 2, 4, 5, 7, 6, 6, 7, 2, 3, 5, 4, 1, 0, 5, 0, 2, 7, 1, 4, 6, 3 };
    cubeMesh.CreateFaceVertexIndicesAttr().Set(faceVertexIndices);

    cubeMesh.AddTranslateOp().Set(i_pos);
    cubeMesh.AddRotateYOp().Set(i_rot);
}

bool uffTest::Read(pxr::SdfLayer* io_layer,
    const std::string& i_resolvedPath,
    bool i_metadataOnly) const 
{
    if (!TF_VERIFY(io_layer)) {
        return false;
    }
    const pxr::SdfFileFormat::FileFormatArguments& args = io_layer->GetFileFormatArguments();
    pxr::SdfAbstractDataRefPtr data = InitData(args);

#define READ_ARG(NAME,TYPE,DEFAULT)\
    TYPE NAME = DEFAULT;\
    if (const std::string* argValue = pxr::TfMapLookupPtr(args, uffParamNames->NAME))\
        NAME = pxr::TfUnstringify<TYPE>(*argValue);
    READ_ARG(randomRotY, float, 0.0f);
    READ_ARG(cubesX, int, 1);
    READ_ARG(cubesY, int, 1);
    READ_ARG(cubesZ, int, 1);
#undef READ_ARG

    pxr::SdfLayerRefPtr newLayerPtr = pxr::SdfLayer::CreateAnonymous(".usd");
    pxr::UsdStageRefPtr stagePtr = pxr::UsdStage::Open(newLayerPtr);

    // All your usd prim definitions here...
    for (int z = 0; z < cubesZ; ++z)
        for (int y = 0; y < cubesY; ++y)
            for (int x = 0; x < cubesX; ++x)
            {
                pxr::SdfPath cubePath(pxr::TfStringPrintf("/uff/cube_%d_%d_%d",x,y,z));
                pxr::GfVec3d cubePos(x*2.0,y*2.0,z*2.0);
                AddMeshCube(stagePtr, cubePath, cubePos, (float(rand()) / float(RAND_MAX)) * randomRotY);
            }

    io_layer->TransferContent(stagePtr->GetRootLayer());

    return true;
}

bool uffTest::ReadFromString(pxr::SdfLayer* io_layer, const std::string& str) const
{
    return false;
}

bool uffTest::WriteToString( const SdfLayer& layer, std::string* str, const std::string& comment) const
{
    return SdfFileFormat::FindById(uffTokens->Id)->WriteToString(layer, str, comment);
}

bool uffTest::WriteToStream( const SdfSpecHandle& spec, std::ostream& out, size_t indent) const
{
    return SdfFileFormat::FindById(uffTokens->Id)->WriteToStream(spec, out, indent);
}

void uffTest::ComposeFieldsForFileFormatArguments(
    const std::string& assetPath,
    const pxr::PcpDynamicFileFormatContext& context,
    pxr::SdfFileFormat::FileFormatArguments* args,
    pxr::VtValue* contextDependencyData) const
{
    pxr::SdfFileFormat::FileFormatArguments outArgs;

    pxr::VtValue val;

    // read params from metadata
    if (context.ComposeValue(uffTokens->Params, &val) && val.IsHolding<VtDictionary>()) 
    {
        const pxr::VtDictionary& dict = val.UncheckedGet<pxr::VtDictionary>();

#define READ_FROM_DICT(NAME,TYPE)\
        if (const pxr::VtValue* dictVal = pxr::TfMapLookupPtr(dict, uffParamNames->NAME))\
            if (dictVal->IsHolding<TYPE>())\
                outArgs[uffParamNames->NAME] = pxr::TfStringify(dictVal->UncheckedGet<TYPE>());
        READ_FROM_DICT(randomRotY, float);
        READ_FROM_DICT(cubesX, int);
        READ_FROM_DICT(cubesY, int);
        READ_FROM_DICT(cubesZ, int);
#undef READ_FROM_DICT

    }

    // read params from attributes on prim
    // these override the ones from metadata
#define READ_FROM_ATTRS(NAME,TYPE)\
    if (context.ComposeAttributeDefaultValue(uffParamNames->NAME, &val) && val.IsHolding<TYPE>())\
        outArgs[uffParamNames->NAME] = pxr::TfStringify(val.UncheckedGet<TYPE>());
    READ_FROM_ATTRS(randomRotY, float);
    READ_FROM_ATTRS(cubesX, int);
    READ_FROM_ATTRS(cubesY, int);
    READ_FROM_ATTRS(cubesZ, int);
#undef READ_FROM_ATTRS

    * args = outArgs;
}

bool uffTest::_ShouldSkipAnonymousReload() const
{
    return false;
}

bool uffTest::_ShouldReadAnonymousLayers() const
{
    return true;
}