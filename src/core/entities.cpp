#include "core/entities.h"
#include "core/game.h"
#include "core/camera.h"
#include "audio/audio_system.h"
#include <cmath>

static bool nearTileBounds(float px, float pz, float txCenterX, float tzCenterZ, float tileSize, float maxDist)
{
    float half = tileSize * 0.5f;
    float minX = txCenterX - half;
    float maxX = txCenterX + half;
    float minZ = tzCenterZ - half;
    float maxZ = tzCenterZ + half;

    float cx = std::fmax(minX, std::fmin(px, maxX));
    float cz = std::fmax(minZ, std::fmin(pz, maxZ));

    float dx = px - cx;
    float dz = pz - cz;
    return (dx * dx + dz * dz) <= (maxDist * maxDist);
}

bool isWalkable(float x, float z)
{
    auto& lvl = gameLevel();
    float tile = lvl.metrics.tile;
    float offX = lvl.metrics.offsetX;
    float offZ = lvl.metrics.offsetZ;

    int tx = (int)((x - offX) / tile);
    int tz = (int)((z - offZ) / tile);

    const auto& data = lvl.map.data();

    if (tz < 0 || tz >= (int)data.size()) return false;
    if (tx < 0 || tx >= (int)data[tz].size()) return false;

    char c = data[tz][tx];
    if (c == 'Z' || c == 'D') return false;

    return true;
}

void updateEntities(float dt)
{
    auto& g = gameContext();
    auto& lvl = gameLevel();
    auto& audio = gameAudio();

    for (auto& en : lvl.enemies)
    {
        if (en.state == STATE_DEAD)
        {
            en.respawnTimer -= dt;
            if (en.respawnTimer <= 0.0f)
            {
                en.state = STATE_IDLE;
                en.hp = 100;
                en.x = en.startX;
                en.z = en.startZ;
                en.hurtTimer = 0.0f;
            }
            continue;
        }

        if (en.hurtTimer > 0.0f) en.hurtTimer -= dt;

        float dx = camX - en.x;
        float dz = camZ - en.z;
        float dist = std::sqrt(dx * dx + dz * dz);

        switch (en.state)
        {
        case STATE_IDLE:
            if (dist < ENEMY_VIEW_DIST) en.state = STATE_CHASE;
            break;

        case STATE_CHASE:
            if (dist < ENEMY_ATTACK_DIST)
            {
                en.state = STATE_ATTACK;
                en.attackCooldown = .25f;
            }
            else if (dist > ENEMY_VIEW_DIST * 1.5f)
            {
                en.state = STATE_IDLE;
            }
            else
            {
                float dirX = dx / dist;
                float dirZ = dz / dist;

                float moveStep = ENEMY_SPEED * dt;

                float nextX = en.x + dirX * moveStep;
                if (isWalkable(nextX, en.z)) en.x = nextX;

                float nextZ = en.z + dirZ * moveStep;
                if (isWalkable(en.x, nextZ)) en.z = nextZ;
            }
            break;

        case STATE_ATTACK:
            if (dist > ENEMY_ATTACK_DIST)
            {
                en.state = STATE_CHASE;
            }
            else
            {
                en.attackCooldown -= dt;
                if (en.attackCooldown <= 0.0f)
                {
                    g.player.health -= 10;
                    en.attackCooldown = 1.0f;
                    g.player.damageAlpha = 1.0f;
                    audioPlayHurt(audio);
                }
            }
            break;

        default:
            break;
        }
    }

    for (auto& item : lvl.items)
    {
        if (!item.active)
        {
            item.respawnTimer -= dt;
            if (item.respawnTimer <= 0.0f) item.active = true;
            continue;
        }

        float dx = camX - item.x;
        float dz = camZ - item.z;

        if (dx * dx + dz * dz < 1.0f)
        {
            if (item.type == ITEM_KEY)
                continue;

            item.active = false;

            if (item.type == ITEM_HEALTH)
            {
                item.respawnTimer = 15.0f;
                g.player.health += 50;
                if (g.player.health > 100) g.player.health = 100;
                g.player.healthAlpha = 1.0f;
            }
            else if (item.type == ITEM_AMMO)
            {
                item.respawnTimer = 15.0f;
                g.player.stamina = 100.0f;
            }
        }
    }
}

void playerTryInteract()
{
    auto &g = gameContext();
    auto &lvl = gameLevel();

    for (auto &item : lvl.items)
    {
        if (!item.active || item.type != ITEM_KEY)
            continue;

        float dx = camX - item.x;
        float dz = camZ - item.z;
        if (dx * dx + dz * dz > 2.25f) // raio de interacao ~= 1.5
            continue;

        item.active = false;
        item.respawnTimer = 999999.0f;
        g.player.hasKey = true;
        break;
    }

    // Interacao com porta: se tiver chave e estiver perto de um tile 'D',
    // destranca convertendo para ',' (chao + teto).
    const float tile = lvl.metrics.tile;
    const float offX = lvl.metrics.offsetX;
    const float offZ = lvl.metrics.offsetZ;
    const int tx0 = (int)((camX - offX) / tile);
    const int tz0 = (int)((camZ - offZ) / tile);

    for (int dz = -1; dz <= 1; ++dz)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            const int tx = tx0 + dx;
            const int tz = tz0 + dz;

            const auto &data = lvl.map.data();
            if (tz < 0 || tz >= (int)data.size())
                continue;
            if (tx < 0 || tx >= (int)data[tz].size())
                continue;
            if (data[tz][tx] != 'D')
                continue;

            float wx, wz;
            lvl.metrics.tileCenter(tx, tz, wx, wz);
            if (!nearTileBounds(camX, camZ, wx, wz, tile, 1.8f))
                continue;

            if (g.player.hasKey)
            {
                lvl.map.setTile(tx, tz, ',');
            }
            return;
        }
    }
}
