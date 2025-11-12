//
// Created by lalit on 8/26/2025.
//

#include "Game.h"

#include <iostream>
#include <raymath.h>

#include "raylib.h"
#include "Weapons.h"
#include "UIManager.h"

#include <filesystem>
#include <fstream>

#include "LevelLoader.h"
namespace fs = std::filesystem;

using namespace std;

Game::Game(std::unordered_map<std::string, nlohmann::json> json) {

    // init game services
    MainUIManager = UIManager(*this);
    MainTileManager = TileManager(*this);
    MainParticleManager = ParticleManager(*this);
    MainCameraManager = CameraManager(*this);
    MainEntityManager = EntityManager(*this);
    MainSoundManager = SoundManager(*this);

    // game speed & timing
    GameSpeed = 1.0f;
    SlowdownTime = 0;
    MaxSlowdownTime = 0;

    Paused = false;

    // resource maps
    Textures = std::unordered_map<std::string, Texture2D>();
    Weapons = std::unordered_map<std::string, Weapon>();
    Shaders = std::unordered_map<std::string, Shader>();
    EnemyWeaponNamesList= std::vector<std::string>();
    BannedWeaponDrops= std::vector<std::string>();
    WeaponPickups = std::vector<WeaponPickup>();

    // extra stuff
    MainPlayer = nullptr;
    CurrentLevelName = "";
    LevelData = json;
    DebugDraw = false;
    ShouldReturn = false;
    UpgradeUI = false;

    SetGameData();
}

void Game::SetGameData() {
    std::string path = "assets\\img";
    for (const auto & entry : fs::directory_iterator(path)) {
        std::string p = entry.path().filename().string();
        p.erase(p.end() - 4, p.end());
        Texture tex = LoadTexture(entry.path().string().c_str());
        Textures.insert({p, tex});
    }
    path = "assets\\shaders";
    for (const auto & entry : fs::directory_iterator(path)) {
        std::string p = entry.path().filename().string();
        p.erase(p.end() - 5, p.end());
        Shader shader = LoadShader("",entry.path().string().c_str());
        Shaders.insert({p, shader});
    }
    path = "assets\\weapondata";
    for (const auto & entry : fs::directory_iterator(path)) {
        std::string p = entry.path().filename().string();
        p.erase(p.end() - 5, p.end());
        std::ifstream g(entry.path().c_str());
        json data = json::parse(g);
        Weapon wep = {};
        if (data.contains("EnemiesCanUse") && data["EnemiesCanUse"].get<bool>())
            EnemyWeaponNamesList.push_back(p);
        if (data.contains("isMelee"))
            wep.isMelee = data["isMelee"].get<bool>();
        if (data.contains("ShakeScreen"))
            wep.ShakeScreen = data["ShakeScreen"].get<bool>();
        if (data.contains("SlowdownOverTime"))
            wep.SlowdownOverTime = data["SlowdownOverTime"].get<bool>();
        if (data.contains("PushbackForce"))
            wep.PushbackForce = data["PushbackForce"].get<float>();
        if (data.contains("BulletLifetime"))
            wep.BulletLifetime = data["BulletLifetime"].get<float>();
        if (data.contains("Spread"))
        {
            wep.SpreadRange[0] = data["Spread"][0].get<int>();
            wep.SpreadRange[1] = data["Spread"][1].get<int>();
        }
        if (data.contains("WeaponWeightSpeedMultiplier"))
            wep.WeaponWeightSpeedMultiplier = data["WeaponWeightSpeedMultiplier"].get<float>();
        if (data.contains("Speed"))
            wep.Speed = data["Speed"].get<float>();
        if (data.contains("WeaponSize"))
            wep.WeaponSize = data["WeaponSize"].get<float>();
        if (data.contains("Ammo"))
            wep.Ammo = data["Ammo"].get<int>();
        if (data.contains("ReloadTime"))
            wep.ReloadTime = data["ReloadTime"].get<double>();
        if (data.contains("Size"))
            wep.Size = {data["Size"][0], data["Size"][1]};
        if (data.contains("Damage"))
            wep.Damage = data["Damage"].get<float>();
        if (data.contains("Cooldown"))
            wep.Cooldown = data["Cooldown"].get<float>();
        if (data.contains("AngleRange"))
            wep.AngleRange = data["AngleRange"].get<float>();
        if (data.contains("Range"))
            wep.Range = data["Range"].get<float>();
        if (data.contains("Bullets"))
            wep.Bullets = data["Bullets"].get<int>();
        if (data.contains("Intensity"))
            wep.Intensity = data["Intensity"].get<float>();
        if (data.contains("texture"))
            wep.texture = data["texture"].get<string>();
        if (data.contains("bullet_tex"))
            wep.BulletTexture = data["bullet_tex"].get<string>();
        if (data.contains("sound"))
            wep.sound = data["sound"].get<string>();
        Weapons.insert({p, wep});
        g.close();
    }

    uOutlineSize = GetShaderLocation(Shaders["outline"], "outlineSize");
    uOutlineColor = GetShaderLocation(Shaders["outline"], "outlineColor");
    uTextureSize = GetShaderLocation(Shaders["outline"], "textureSize");
    uThreshold = GetShaderLocation(Shaders["outline"], "threshold");

    WeaponPickupTex = LoadRenderTexture(150, 150);

    BannedWeaponDrops.emplace_back("Default Gun");
    BannedWeaponDrops.emplace_back("Player Gun");
}

void Game::Slowdown(float Time) {
    SlowdownTime = Time;
    MaxSlowdownTime = Time;
}

void Game::Slowdown(float Time, float CrashIntensity) {
    SlowdownTime = Time;
    MaxSlowdownTime = Time;
    SlowdownShakeIntensity = CrashIntensity;
}

void Game::ProcessSlowdownAnimation() {
    if (SlowdownTime > 0 && MaxSlowdownTime > 0) {
        float Percent = SlowdownTime / MaxSlowdownTime;
        if (Percent < 0.5)
            GameSpeed = Lerp(1.0f, 0.1f, Percent-0.5f);
        else
            GameSpeed = Lerp(0.1f, 1.0f, Percent-0.5f);
        if (SlowdownShakeIntensity > 0 && Percent < 0.5f) {
            MainCameraManager.ShakeCamera(SlowdownShakeIntensity);
            MainSoundManager.PlaySoundM("dash_hit", min(max(SlowdownShakeIntensity, 0.0f), 1.0f));
            SlowdownShakeIntensity = 0;
        }
        SlowdownTime -= GetFrameTime();
    } else {
        SlowdownTime = 0;
        MaxSlowdownTime = 0;
        SlowdownShakeIntensity = 0;
        GameSpeed = 1.0f;
    }
}

void Game::PlaceWeaponPickup(WeaponPickup pickup) {
    for (std::string s : BannedWeaponDrops)
    {
        if (s == pickup.Weapon)
            return;
    }
    pickup.CreationTime = GetTime();
    WeaponPickups.push_back(pickup);
}

void Game::DisplayPickups(Camera2D cam)
{
    std::erase_if(WeaponPickups, [&](WeaponPickup& pickup) {
            return pickup.PickedUp || GetTime() - pickup.CreationTime >= 45 || !Weapons.contains(pickup.Weapon);
    });
    for (WeaponPickup& pickup : WeaponPickups) {
        // get floating offset
        float AnimationOffset = sin((GetTime() - pickup.CreationTime) * pickup.AnimationSpeed) * pickup.AnimationPower;
        Weapon& PickupWeapon = Weapons.at(pickup.Weapon);
        std::string TexString = "placeholder";
        if (Textures.contains(PickupWeapon.texture))
            TexString=PickupWeapon.texture;

        Vector2 siz = {(float)Textures[TexString].width, (float)Textures[TexString].height};
        siz = Vector2Normalize(siz);
        siz = Vector2Multiply(siz, {pickup.Radius*2.5f, pickup.Radius*2.5f});

        float outlineSize = 3.0f;
        float threshold = 0.5f;
        Color outlineColor = pickup.PickupColor;

        EndTextureMode();
        EndMode2D();
        BeginTextureMode(WeaponPickupTex);
        ClearBackground(BLANK);

        BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);

        DrawTexturePro(Textures[TexString], {
            0,
            0,
            (float)Textures[TexString].width,
            (float)Textures[TexString].height
        }, {
            WeaponPickupTex.texture.width/2.0f,WeaponPickupTex.texture.height/2.0f,
            siz.x,
            siz.y

        }, {siz.x / 2, siz.y / 2}, 0, WHITE);
        EndBlendMode();
        EndTextureMode();
        BeginTextureMode(MainCameraManager.CameraRenderTexture);
        BeginMode2D(cam);

        BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
        BeginShaderMode(Shaders["outline"]);
        SetShaderValue(Shaders["outline"], uTextureSize, new float[2] {(float)WeaponPickupTex.texture.width,
            (float)WeaponPickupTex.texture.height}, SHADER_UNIFORM_VEC2);
        SetShaderValue(Shaders["outline"], uThreshold, &threshold, SHADER_UNIFORM_FLOAT);
        SetShaderValue(Shaders["outline"], uOutlineSize, &outlineSize, SHADER_UNIFORM_FLOAT);
        SetShaderValue(Shaders["outline"], uOutlineColor, new float[4] {
            (float)outlineColor.r / 255.0f,
                (float)outlineColor.g / 255.0f,
                (float)outlineColor.b / 255.0f,
                    (float)outlineColor.a / 255.0f
        }, SHADER_UNIFORM_VEC4);
        DrawTexturePro(WeaponPickupTex.texture, {0,0,(float)WeaponPickupTex.texture.width,
            (float)-WeaponPickupTex.texture.height
        }, {pickup.Position.x - MainCameraManager.CameraPosition.x,
            pickup.Position.y  - MainCameraManager.CameraPosition.y - AnimationOffset,
            (float)WeaponPickupTex.texture.width,
                (float)WeaponPickupTex.texture.height},
            {WeaponPickupTex.texture.width/2.0f,WeaponPickupTex.texture.height/2.0f}, 0, WHITE);
        EndBlendMode();
        EndShaderMode();

        if (DebugDraw)
            DrawCircleV({pickup.Position.x - MainCameraManager.CameraPosition.x,
            pickup.Position.y  - MainCameraManager.CameraPosition.y - AnimationOffset}, pickup.Radius, ColorAlpha(RED, 0.5f));

        // get distance
        float DistanceToPickup = Vector2Distance(pickup.Position, {
            MainPlayer->BoundingBox.x,
            MainPlayer->BoundingBox.y
        });

        // in range?
        if (DistanceToPickup <= pickup.Radius)
        {
            pickup.PickedUp = MainPlayer->weaponsSystem.GiveWeapon(pickup.Weapon);
        }
    }
}

void Game::Update(Camera2D camera) {
    if (IsKeyPressed(KEY_M))
        Paused = !Paused;

    if (!Paused) {
        if (IsKeyPressed(KEY_X))
            DebugDraw = !DebugDraw;

        // shader stuff
        if (!DebugDraw)
            MainCameraManager.ShaderDraw = false;
        if (IsKeyPressed(KEY_C) && DebugDraw)
            MainCameraManager.ShaderDraw = !MainCameraManager.ShaderDraw;
        if (IsKeyDown(KEY_B) && MainCameraManager.ShaderDraw)
            MainCameraManager.ShaderPixelPower += 10 * GetFrameTime();
        if (IsKeyDown(KEY_N) && MainCameraManager.ShaderDraw)
            MainCameraManager.ShaderPixelPower -= 10 * GetFrameTime();

        if (IsKeyPressed(KEY_E))
        {
            if (MainEntityManager.Entities[EnemyType].size() <= 0)
                ShouldReturn = true;
            if ((MainPlayer->Health <= 0 || MainPlayer->ShouldDelete) && !CurrentLevelName.empty())
                Reload(CurrentLevelName);
        }

        MainCameraManager.Begin(camera);
        ProcessSlowdownAnimation();
        MainTileManager.Update();
        MainParticleManager.Update();
        MainEntityManager.Update();
        MainSoundManager.Update();
        DisplayPickups(camera);
        MainCameraManager.End();

    }

    MainCameraManager.Display();
    MainUIManager.GameUI();
}

bool Game::RayCast(Vector2 origin, Vector2 target) {

    Vector2 vRayStart = origin;
    Vector2 vRayDir = Vector2Normalize(target - origin);
    Vector2 vRayUnitStepSize = { sqrt(1 + (vRayDir.y / vRayDir.x) * (vRayDir.y / vRayDir.x)), sqrt(1 + (vRayDir.x / vRayDir.y) * (vRayDir.x / vRayDir.y)) };
    Vector2 vMapCheck = origin;
    Vector2 vRayLength1D;
    Vector2 vStep;

    // Establish Starting Conditions
    if (vRayDir.x < 0)
    {
        vStep.x = -1;
        vRayLength1D.x = (vRayStart.x - float(vMapCheck.x)) * vRayUnitStepSize.x;
    }
    else
    {
        vStep.x = 1;
        vRayLength1D.x = (float(vMapCheck.x + 1) - vRayStart.x) * vRayUnitStepSize.x;
    }

    if (vRayDir.y < 0)
    {
        vStep.y = -1;
        vRayLength1D.y = (vRayStart.y - float(vMapCheck.y)) * vRayUnitStepSize.y;
    }
    else
    {
        vStep.y = 1;
        vRayLength1D.y = (float(vMapCheck.y + 1) - vRayStart.y) * vRayUnitStepSize.y;
    }

    // Perform "Walk" until collision or range check
    bool bTileFound = false;
    float fMaxDistance = Vector2Distance(origin,target);
    float fDistance = 0.0f;
    while (!bTileFound && fDistance < fMaxDistance)
    {
        // Walk along shortest path
        if (vRayLength1D.x < vRayLength1D.y)
        {
            vMapCheck.x += vStep.x;
            fDistance = vRayLength1D.x;
            vRayLength1D.x += vRayUnitStepSize.x;
        }
        else
        {
            vMapCheck.y += vStep.y;
            fDistance = vRayLength1D.y;
            vRayLength1D.y += vRayUnitStepSize.y;
        }

        // Test tile at new test point
        if (vMapCheck.x >= 0 && vMapCheck.x < MainTileManager.MapWidth*MainTileManager.TileSize && vMapCheck.y >= 0 && vMapCheck.y < MainTileManager.MapHeight*MainTileManager.TileSize)
        {
            std::string s = to_string((int)(vMapCheck.x/MainTileManager.TileSize)) + " " + to_string((int)(vMapCheck.y/MainTileManager.TileSize));
            int g = MainTileManager.Map[s];
            // cout << s << " " << g << endl;

            if (MainTileManager.TileTypes[g]==WallTileType)
            {
                bTileFound = true;
                break;
            }
        }
    }

    /*
    // Calculate intersection location
    Vector2 vIntersection;
    if (bTileFound)
    {
        vIntersection = vRayStart + vRayDir * fDistance;
    }
    */
    return !bTileFound;
}


bool Game::RayCastSP(Vector2 origin, Vector2 target, float Precision) { // RayCasting (old)function

    if (Precision <= 0)
        Precision = Vector2Distance(origin, target) / 15.0f;

    // Init starting positions
    float ray_x = origin.x;
    float ray_y = origin.y;
    auto start_distance = static_cast<float>(sqrt(pow(ray_x - target.x, 2) + pow(ray_y - target.y, 2)));

    // Getting the step distance

    const float step_x = ((target.x-ray_x) / start_distance) * Precision;
    const float step_y = ((target.y-ray_y) / start_distance) * Precision;

    // Getting the side we are on relative to the target

    bool x_side = (target.x - ray_x) < 0;
    bool y_side = (target.y - ray_y) < 0;

    // marching through the world, check if we collide with something
    while (sqrt(pow(ray_x - target.x, 2) + pow(ray_y - target.y, 2)) > 2) {
        ray_x += step_x;
        ray_y += step_y;
        if (DebugDraw)
            DrawCircle((int)ray_x-MainCameraManager.CameraPosition.x, (int)ray_y-MainCameraManager.CameraPosition.y, 2, RED);
        std::string coord = std::to_string((int) (ray_x / MainTileManager.TileSize)) + " " + std::to_string((int) (ray_y / MainTileManager.TileSize));
        // if we collide with a wall, raycast failed
        if (int tile_id = MainTileManager.Map[coord]; MainTileManager.TileTypes[tile_id] == WallTileType) {
            return false;
        }
        bool curr_x_side = (target.x - ray_x) < 0;
        bool curr_y_side = (target.y - ray_y) < 0;
        if (curr_x_side != x_side || curr_y_side != y_side) { // Have we traveled beyond the target? havent found anything? If so, exit!!
            return true;
        }
    }
    return true;
}

void Game::Clear() {
    Paused = false;
    ShouldReturn = false;
    CurrentLevelName.clear();
    WeaponPickups.clear();
    MainTileManager.Clear();
    MainParticleManager.Clear();
    MainEntityManager.Clear();
    MainSoundManager.Clear();
    MainCameraManager.Clear();
    MainPlayer.reset();
}

void Game::Reload(std::string MapName) {

    Clear();

    CurrentLevelName = MapName;

    MainTileManager.ReadMapDataFile("assets\\maps\\" + CurrentLevelName + "\\map_data.csv");

    MainPlayer = make_shared<Player>(MainTileManager.PlayerSpawnPosition.x,
                                     MainTileManager.PlayerSpawnPosition.y, 350.0f,
                                     Textures["player"], *this);
    MainPlayer->MaxHealth = LevelData[MapName]["starting_health"];
    MainPlayer->Health = LevelData[MapName]["starting_health"];
    MainEntityManager.AddEntity(PlayerType, MainPlayer);
}

void Game::UnloadAssets() {
    for (auto& [name,value] : Textures)
        UnloadTexture(value);
    for (auto& [name,value] : Shaders)
        UnloadShader(value);
    UnloadRenderTexture(WeaponPickupTex);
}

void Game::Quit() {
    Clear();
    MainTileManager.Quit();
    MainUIManager.Quit();
    MainParticleManager.Quit();
    MainCameraManager.Quit();
    MainSoundManager.Quit();
    MainEntityManager.Quit();
    UnloadAssets();
}