package com.greedybreaker.repository;

import com.greedybreaker.model.ReproducibilityTokenEntity;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface ReproducibilityTokenRepository extends JpaRepository<ReproducibilityTokenEntity, String> {
    Optional<ReproducibilityTokenEntity> findByExecutionId(String executionId);
}
