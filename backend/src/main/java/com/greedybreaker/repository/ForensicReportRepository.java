package com.greedybreaker.repository;

import com.greedybreaker.model.ForensicReportEntity;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface ForensicReportRepository extends JpaRepository<ForensicReportEntity, String> {
    Optional<ForensicReportEntity> findByExecutionId(String executionId);
}
